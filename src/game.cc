#include <stdio.h>
#include <vector>
#include <cmath>
#include <ctime>
#include <thread>
#include <cstring>
#include <algorithm>

#include "../include/GameStatus.h"
#include "../include/PrisonMap.h"
#include "../include/config.h"
#include "../include/Camera.h"
#include "../include/Pathfinding/astar.h"
#include "../include/UI/SimulationSpeedControls.h"
#include "../include/UI/FpsCounter.h"
#include "../include/UI/VSyncToggle.h"
#include "../include/UI/InfoPanel.h"
#include "../include/ecs/components/PawnComponents.h"
#include "../include/ecs/components/TransformComponent.h"
#include "../include/UI/InfoPanel.h"
#include "../include/ecs/PawnEcsSystems.h"
#include "../include/ecs/PawnFactory.h"
#include "../include/ecs/PawnSelection.h"
#include "../include/Map/ResourceTypeManager.h"
#include <MOMOS/momos.h>
#include <MOMOS/draw.h>
#include <MOMOS/input.h>

#include "../include/Agents/Pathfinder.h"


SimulationSpeedControls g_speed_controls;
FpsCounter g_fps_counter;
VSyncToggle g_vsync_toggle;

namespace {

} // namespace


/// Process user input
void Input() {
	g_speed_controls.HandleInput();

	static double last_input_time = 0.0;
	double current_time = MOMOS::Time();
	if (last_input_time == 0.0) {
		last_input_time = current_time;
	}
	float delta_seconds = static_cast<float>(current_time - last_input_time);
	if (delta_seconds < 0.0f) {
		delta_seconds = 0.0f;
	}
	last_input_time = current_time;

	if (MOMOS::MouseButtonDown(1)) {
		float mx = static_cast<float>(MOMOS::MousePositionX());
		float my = static_cast<float>(MOMOS::MousePositionY());
		g_vsync_toggle.HandleClick(mx, my);
		
		// Only process clicks when button is first pressed (MouseButtonDown)
		::MOMOS::Vec2 mouse_screen = {
			static_cast<float>(MOMOS::MousePositionX()),
			static_cast<float>(MOMOS::MousePositionY())
		};
		
		bool pawn_clicked = PawnSelection::HandleClick();
		if (pawn_clicked) {
			// Clear cell selection when pawn is clicked
			if (GameStatus::get()->map) {
				GameStatus::get()->map->ClearCellSelection();
			}
		} else {
			// Clear pawn selection when clicking elsewhere
			PawnSelection::ClearSelection();
			InfoPanel::Get().SetSelectedPawn(ECS::Entity());
			
			// Try to click on a cell
			if (GameStatus::get()->map) {
				GameStatus::get()->map->HandleCellClick(mouse_screen);
			}
		}
	}

	Camera::HandleInput(delta_seconds);
}


/// Updates agents. If acummTime runs out, it will keep on calculating next frame where it left of.
void UpdateAI(double accumTime) {
	//Keep track of remaining time left on this frame
	double start, end;
	bool timeout = false;
	int j = (Agent::last_updated_id_ == -1) ? 0 : Agent::last_updated_id_;

	for (unsigned int i = j; i < Agent::agents_.size() && !timeout; i++) {
		Agent* agent = Agent::agents_[i];

		start = GameStatus::get()->game_time;
		agent->update(accumTime);
		end = GameStatus::get()->game_time;

		if (agent->getBody() && agent->getBody()->pos_.y > Screen::height)
			agent->aliveStatus_ = kDead;

		accumTime -= end - start;
		if (accumTime <= 0.0f) {
			Agent::last_updated_id_ = i;
			timeout = true;
		}

		//Once the last agent has been updated, return to the first one if there's still time
		if (i == j - 1) {
			i = 0;
		}
	}

	if (!timeout) {
		Agent::last_updated_id_ = -1;
	}
}


void Draw() {
	MOMOS::DrawBegin();
	MOMOS::DrawClear(200, 200, 200);

	//Draw map background image
	GameStatus::get()->map->Draw();

	//Draw agents
	for (unsigned int i = 0; i < Agent::agents_.size(); i++) {
		//Skip dead agents and managers (agents with no body)
		if (Agent::agents_[i]->aliveStatus_ == kAlive && Agent::agents_[i]->getBody()) {
			Agent::agents_[i]->render();
		}
	}

	PawnECS::Systems::Get().Render(0.0);
	PawnSelection::DrawSelection();
	if (GameStatus::get()->map) {
		GameStatus::get()->map->DrawCellSelection();
	}
	g_fps_counter.Draw();
	g_vsync_toggle.Draw(g_fps_counter.GetTextRight(), g_fps_counter.GetTextBaselineY());
	g_speed_controls.Draw();
	InfoPanel::Get().Draw();

	MOMOS::DrawEnd();
	MOMOS::WindowFrame();
}


/** Checks and returns if the agents have been created and the simulation has started. **/
bool checkGameStarted() {
	auto* status = GameStatus::get();
	if (!status->pawns_created) {
		status->pawns_created = true;

		// Pathfinder manager should be the first one to update each frame
		if (status->pathfinder_) {
			Agent::agents_.push_back(status->pathfinder_);
		}

		const int total_pawns = 100;
		for (int i = 0; i < total_pawns; ++i) {
			PawnECS::SpawnPawn();
		}
	}

	return status->pawns_created;
}


/// Main update loop
void Update(double m_iTimeStep) {
	double effective_step = m_iTimeStep * GameStatus::get()->simulation_speed_;
	GameStatus::get()->game_time += effective_step;

	bool started = checkGameStarted();
	if (started && effective_step > 0.0) {
		UpdateAI(effective_step);
	}

	PawnECS::Systems::Get().Update(effective_step);
}


/// Entry point
int game(int argc, char** argv) {

	srand(static_cast<unsigned int>(time(NULL)));

	MOMOS::WindowInit(Screen::width, Screen::height);
	Camera::Initialize();

	::MOMOS::Vec2 initial_zoom_focus = {
		static_cast<float>(Screen::width) * 0.5f,
		static_cast<float>(Screen::height) * 0.5f
	};
	Camera::ZoomBy(Camera::kZoomStep * 2.0f, initial_zoom_focus);
	g_vsync_toggle.Initialize(false);

	// Load resource types from JSON
	ResourceTypeManager::Get().LoadFromJSON("data/resource_types.json");

	//Init variables and locations for this specific map
	GameStatus::get()->prison = new PrisonMap();

	GameStatus::get()->map = new CostMap();
	GameStatus::get()->map->Load("data/map_03_60x44_bw.bmp", "data/map_03_960x704_layoutAB.bmp");
	GameStatus::get()->pathfinder_ = new Pathfinder();
	g_speed_controls.Initialize();

	PrisonMap* prison = GameStatus::get()->prison;

	MOMOS::DrawSetTextFont("data/medieval.ttf");
	MOMOS::WindowSetMouseVisibility(true);
	MOMOS::DrawSetFillColor(200, 50, 100, 255);

	// 40ms per frame
	float m_iTimeStep = 16.0f;
	double CurrentTime = MOMOS::Time();

	checkGameStarted();

	// Game loop with fixed update
	while (MOMOS::WindowIsOpened()) {

		Input();

		g_fps_counter.Update();

		double accumTime = MOMOS::Time() - CurrentTime;
		while (accumTime >= m_iTimeStep) {
			Update(m_iTimeStep);
			CurrentTime += m_iTimeStep;
			accumTime = MOMOS::Time() - CurrentTime;
		}

		Draw();
	}

	return 0;
}