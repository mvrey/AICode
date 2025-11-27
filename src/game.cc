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
#include <MOMOS/momos.h>
#include <MOMOS/draw.h>

#include "../include/Agents/Pathfinder.h"


SimulationSpeedControls g_speed_controls;
FpsCounter g_fps_counter;
VSyncToggle g_vsync_toggle;

namespace {

	ECS::Entity g_selected_pawn;
	::MOMOS::Vec2 g_selected_cell = { -1.0f, -1.0f }; // Invalid cell position


constexpr float kCameraPanSpeed = 0.5f;
constexpr float kEdgePanPadding = 20.0f;
constexpr float kZoomStep = 0.25f;

void HandleCameraZoom() {
	float wheel_delta = static_cast<float>(MOMOS::MouseWheelY());
	if (wheel_delta == 0.0f) {
		return;
	}

	::MOMOS::Vec2 mouse_screen_position = {
		static_cast<float>(MOMOS::MousePositionX()),
		static_cast<float>(MOMOS::MousePositionY())
	};

	Camera::ZoomBy(wheel_delta * kZoomStep, mouse_screen_position);
}

void HandleCameraPan(float delta_seconds) {
	if (delta_seconds <= 0.0f || !Camera::CanPan()) {
		return;
	}

	::MOMOS::Vec2 direction = { 0.0f, 0.0f };

	if (MOMOS::IsKeyPressed('A') || MOMOS::IsSpecialKeyPressed(MOMOS::kSpecialKey_Left)) {
		direction.x -= 1.0f;
	}
	if (MOMOS::IsKeyPressed('D') || MOMOS::IsSpecialKeyPressed(MOMOS::kSpecialKey_Right)) {
		direction.x += 1.0f;
	}
	if (MOMOS::IsKeyPressed('W') || MOMOS::IsSpecialKeyPressed(MOMOS::kSpecialKey_Up)) {
		direction.y -= 1.0f;
	}
	if (MOMOS::IsKeyPressed('S') || MOMOS::IsSpecialKeyPressed(MOMOS::kSpecialKey_Down)) {
		direction.y += 1.0f;
	}

	float mouse_x = static_cast<float>(MOMOS::MousePositionX());
	float mouse_y = static_cast<float>(MOMOS::MousePositionY());

	if (mouse_x < kEdgePanPadding) {
		direction.x -= 1.0f;
	} else if (mouse_x > Screen::width - kEdgePanPadding) {
		direction.x += 1.0f;
	}

	if (mouse_y < kEdgePanPadding) {
		direction.y -= 1.0f;
	} else if (mouse_y > Screen::height - kEdgePanPadding) {
		direction.y += 1.0f;
	}

	float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	if (magnitude <= 0.0f) {
		return;
	}

	direction.x /= magnitude;
	direction.y /= magnitude;

	float distance = kCameraPanSpeed * delta_seconds;
	Camera::Pan(::MOMOS::Vec2{ direction.x * distance, direction.y * distance });
}

bool HandlePawnClick() {
	if (!MOMOS::MouseButtonDown(1)) {
		return false;
	}

	::MOMOS::Vec2 mouse_screen = {
		static_cast<float>(MOMOS::MousePositionX()),
		static_cast<float>(MOMOS::MousePositionY())
	};
	::MOMOS::Vec2 world_click = Camera::ScreenToWorld(mouse_screen);

	auto& registry = PawnECS::GetRegistry();
	const float click_radius = 32.0f;
	float best_distance_sq = click_radius * click_radius;
	bool found = false;
	std::string closest_name;
	struct PawnClickBreak {};

	try {
		registry.ForEach<ECS::PawnStateComponent>([&](ECS::Entity entity, ECS::PawnStateComponent& state) {
			if (!registry.HasComponent<ECS::TransformComponent>(entity)) {
				return;
			}
			auto& transform = registry.GetComponent<ECS::TransformComponent>(entity);
			float dx = transform.position.x - world_click.x;
			float dy = transform.position.y - world_click.y;
			float distance_sq = dx * dx + dy * dy;
			if (distance_sq <= best_distance_sq) {
				best_distance_sq = distance_sq;
				found = true;
				closest_name = state.name.empty() ? "Unnamed Pawn" : state.name;
				g_selected_pawn = entity;
				throw PawnClickBreak();
			}
		});
	} catch (const PawnClickBreak&) {
	}

	if (found) {
		InfoPanel::Get().SetMessage(closest_name);
		g_selected_cell = { -1.0f, -1.0f }; // Clear cell selection when pawn is clicked
		return true;
	}
	
	// Clear selection if no pawn was found
	g_selected_pawn = ECS::Entity();
	return false;
}

void HandleCellClick() {
	if (!MOMOS::MouseButtonDown(1)) {
		return;
	}

	::MOMOS::Vec2 mouse_screen = {
		static_cast<float>(MOMOS::MousePositionX()),
		static_cast<float>(MOMOS::MousePositionY())
	};

	CostMap* map = GameStatus::get()->map;
	if (map == nullptr) {
		return;
	}

	// Convert screen coordinates to world coordinates (accounting for camera)
	::MOMOS::Vec2 world_click = Camera::ScreenToWorld(mouse_screen);

	// Calculate tile world dimensions (same as in CostMap::Draw)
	float tile_world_width = static_cast<float>(Screen::width) / static_cast<float>(map->getWidth());
	float tile_world_height = static_cast<float>(Screen::height) / static_cast<float>(map->getHeight());

	// Convert world coordinates to map coordinates
	int map_x = static_cast<int>(world_click.x / tile_world_width);
	int map_y = static_cast<int>(world_click.y / tile_world_height);

	// Clamp to valid map bounds
	map_x = std::max(0, std::min(map_x, map->getWidth() - 1));
	map_y = std::max(0, std::min(map_y, map->getHeight() - 1));

	// Get the cell at the clicked position
	Cell* cell = map->getCellAt(map_x, map_y);
	if (cell == nullptr) {
		return;
	}

	// Store selected cell position
	g_selected_cell = { static_cast<float>(map_x), static_cast<float>(map_y) };

	// Format and display the cell cost
	char cost_text[64];
	snprintf(cost_text, sizeof(cost_text), "Cell Cost: %.2f", cell->cost_);
	InfoPanel::Get().SetMessage(cost_text);
}

void DrawPawnSelection() {
	if (!g_selected_pawn.IsValid()) {
		return;
	}

	auto& registry = PawnECS::GetRegistry();
	if (!registry.HasComponent<ECS::TransformComponent>(g_selected_pawn)) {
		return;
	}

	auto& transform = registry.GetComponent<ECS::TransformComponent>(g_selected_pawn);
	const float half_screen = 16.0f;
	const float half_world = half_screen / Camera::Zoom();

	::MOMOS::Vec2 top_left = {
		transform.position.x - half_world,
		transform.position.y - half_world
	};
	::MOMOS::Vec2 bottom_right = {
		transform.position.x + half_world,
		transform.position.y + half_world
	};

	auto top_left_screen = Camera::WorldToScreen(top_left);
	auto bottom_right_screen = Camera::WorldToScreen(bottom_right);

	MOMOS::DrawSetStrokeColor(100, 255, 100, 255);
	MOMOS::DrawLine(top_left_screen.x, top_left_screen.y, bottom_right_screen.x, top_left_screen.y);
	MOMOS::DrawLine(bottom_right_screen.x, top_left_screen.y, bottom_right_screen.x, bottom_right_screen.y);
	MOMOS::DrawLine(bottom_right_screen.x, bottom_right_screen.y, top_left_screen.x, bottom_right_screen.y);
	MOMOS::DrawLine(top_left_screen.x, bottom_right_screen.y, top_left_screen.x, top_left_screen.y);
}

void DrawCellSelection() {
	// Check if a cell is selected (valid position)
	if (g_selected_cell.x < 0.0f || g_selected_cell.y < 0.0f) {
		return;
	}

	CostMap* map = GameStatus::get()->map;
	if (map == nullptr) {
		return;
	}

	// Calculate tile world dimensions
	float tile_world_width = static_cast<float>(Screen::width) / static_cast<float>(map->getWidth());
	float tile_world_height = static_cast<float>(Screen::height) / static_cast<float>(map->getHeight());

	// Get cell position in world coordinates
	int cell_x = static_cast<int>(g_selected_cell.x);
	int cell_y = static_cast<int>(g_selected_cell.y);

	// Calculate world bounds of the cell
	::MOMOS::Vec2 world_top_left = {
		cell_x * tile_world_width,
		cell_y * tile_world_height
	};
	::MOMOS::Vec2 world_bottom_right = {
		(cell_x + 1) * tile_world_width,
		(cell_y + 1) * tile_world_height
	};

	// Convert to screen coordinates
	::MOMOS::Vec2 top_left_screen = Camera::WorldToScreen(world_top_left);
	::MOMOS::Vec2 bottom_right_screen = Camera::WorldToScreen(world_bottom_right);

	// Draw green square around the cell
	MOMOS::DrawSetStrokeColor(100, 255, 100, 255);
	MOMOS::DrawLine(top_left_screen.x, top_left_screen.y, bottom_right_screen.x, top_left_screen.y);
	MOMOS::DrawLine(bottom_right_screen.x, top_left_screen.y, bottom_right_screen.x, bottom_right_screen.y);
	MOMOS::DrawLine(bottom_right_screen.x, bottom_right_screen.y, top_left_screen.x, bottom_right_screen.y);
	MOMOS::DrawLine(top_left_screen.x, bottom_right_screen.y, top_left_screen.x, top_left_screen.y);
}

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
	}
	bool pawn_clicked = HandlePawnClick();
	if (!pawn_clicked) {
		HandleCellClick();
	}

	HandleCameraZoom();
	HandleCameraPan(delta_seconds);
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
	DrawPawnSelection();
	DrawCellSelection();
	g_fps_counter.Draw();
	g_vsync_toggle.Draw(g_fps_counter.GetTextRight(), g_fps_counter.GetTextBaselineY());
	g_speed_controls.Draw();
	InfoPanel::Get().Draw();

	MOMOS::DrawEnd();
	MOMOS::WindowFrame();
}


/** Checks and returns if the agents have been created and the simulation has started.
	It also creates the agents if the g_game_mode has been set.
 **/
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
	Camera::ZoomBy(kZoomStep * 2.0f, initial_zoom_focus);
	g_vsync_toggle.Initialize(false);

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