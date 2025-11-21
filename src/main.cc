#include <stdio.h>
#include <vector>
#include <ctime>
#include <thread>
#include <cstring>

#include "../include/GameStatus.h"
#include "../include/PrisonMap.h"
#include "../include/config.h"
#include "../include/Pathfinding/astar.h"
#include "../include/SimulationSpeedControls.h"
#include "../include/FpsCounter.h"
#include "../include/VSyncToggle.h"
#include <MOMOS/momos.h>

#include "../include/Agents/Soldier.h"
#include "../include/Agents/Guard.h"
#include "../include/Agents/Prisoner.h"
#include "../include/Agents/Pathfinder.h"


// Global vars
double g_shift_change_time_end = 0;
MOMOS::SpriteHandle g_shift_change_img;
MOMOS::SpriteHandle g_alarm_mode_img;

SimulationSpeedControls g_speed_controls;
FpsCounter g_fps_counter;
VSyncToggle g_vsync_toggle;


/// Process user input
void Input() {
	g_speed_controls.HandleInput();
	if (MOMOS::MouseButtonDown(1)) {
		float mx = static_cast<float>(MOMOS::MousePositionX());
		float my = static_cast<float>(MOMOS::MousePositionY());
		g_vsync_toggle.HandleClick(mx, my);
	}
}


/// Updates agents. If acummTime runs out, it will keep on calculating next frame where it left of.
void UpdateAI(double accumTime) {
	//Keep track of remaining time left on this frame
	double start, end;
	bool timeout = false;
	int j = (Agent::last_updated_id_ == -1) ? 0 : Agent::last_updated_id_;

	for (unsigned int i = j; i < Agent::agents_.size() && !timeout; i++) {
		start = GameStatus::get()->game_time;
		Agent::agents_[i]->update(accumTime);
		end = GameStatus::get()->game_time;

		if (Agent::agents_[i]->getBody() && Agent::agents_[i]->getBody()->pos_.y > Screen::height)
			Agent::agents_[i]->aliveStatus_ = kDead;

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

	//Draw crates
	std::vector<Crate*> crates = GameStatus::get()->crates_;
	for (unsigned int i = 0; i < crates.size(); i++) {
		MOMOS::Vec2 map_crates = crates[i]->pos_;
		MOMOS::DrawSprite(crates[i]->img_, map_crates.x, map_crates.y);
	}


	//Draw doors
	std::vector<Door*> doors = GameStatus::get()->prison->doors_;
	for (unsigned int i = 0; i < doors.size(); i++) {
		MOMOS::Vec2 door_map_coords = GameStatus::get()->map->MapToScreenCoords(doors[i]->pos_);
		MOMOS::DrawSprite(doors[i]->getImg(), door_map_coords.x, door_map_coords.y);
	}

	//Draw ALARM icon
	if (GameStatus::get()->alarm_mode_) {
		MOMOS::DrawSprite(g_alarm_mode_img, 20.0f, 50.0f);
	}
	//Draw shift change icon
	else if (g_shift_change_time_end > GameStatus::get()->game_time) {
		MOMOS::DrawSprite(g_shift_change_img, 20.0f, 50.0f);
	}

	g_fps_counter.Draw();
	g_vsync_toggle.Draw(g_fps_counter.GetTextRight(), g_fps_counter.GetTextBaselineY());
	g_speed_controls.Draw();

	MOMOS::DrawEnd();
	MOMOS::WindowFrame();
}


/** Checks and returns if the agents have been created and the simulation has started. 
	It also creates the agents if the g_game_mode has been set. 
 **/
bool checkGameStarted() {

	if (!GameStatus::get()->g_agents_created) {
		GameStatus::get()->g_agents_created = true;

		//Pathfinder manager should be the 1st one to update each frame
		Agent::agents_.push_back(GameStatus::get()->pathfinder_);

		//Create prison guards
		for (int i = 0; i < 10; i++) {
			Guard* agent = new Guard();
			Agent::agents_.push_back(agent);
			GameStatus::get()->guards_.push_back(agent);
		}

		//Create prisoners
		for (int i = 0; i < 10; i++) {
			Prisoner* agent = new Prisoner();

			if (i > 10/2)
				agent->working_shift_ = 1;

			Agent::agents_.push_back(agent);
			GameStatus::get()->prisoners_.push_back(agent);
		}

		//Create soldiers
		for (int i = 0; i < 10; i++) {
			Soldier* agent = new Soldier();
			agent->getBody()->pos_ = { (float)(rand() % 200) + 30.0f, Screen::height - 50.0f };
			Agent::agents_.push_back(agent);
			GameStatus::get()->soldiers_.push_back(agent);
		}
	}

	return GameStatus::get()->g_agents_created;
}


/// Main update loop
void Update(double m_iTimeStep) {
	double effective_step = m_iTimeStep * GameStatus::get()->simulation_speed_;
	GameStatus::get()->game_time += effective_step;

	//Check working shift
	if (GameStatus::get()->working_shift_time_end < GameStatus::get()->game_time) {
		GameStatus::get()->working_shift_time_end += 20000.0;
		GameStatus::get()->working_shift_ = (GameStatus::get()->working_shift_ == 0) ? 1 : 0;
		g_shift_change_time_end = GameStatus::get()->game_time + 3000.0;
	}

	//Check alarm mode
	if (GameStatus::get()->alarm_mode_ && GameStatus::get()->alarm_mode_time_end_ <= GameStatus::get()->game_time) {
		GameStatus::get()->alarm_mode_ = false;
	}

	bool started = checkGameStarted();
	if (started && effective_step > 0.0) {
		UpdateAI(effective_step);
	}

	//Alarm cheat
	if (MOMOS::IsKeyPressed('L')) {
		GameStatus::get()->alarm_mode_ = true;
		GameStatus::get()->alarm_mode_time_end_ = GameStatus::get()->game_time + 10000.0;
	}
}


/// Entry point
int main(int argc, char **argv) {
	
	srand(static_cast<unsigned int>(time(NULL)));

	MOMOS::WindowInit(Screen::width, Screen::height);
	g_vsync_toggle.Initialize(false);

	//Init variables and locations for this specific map
	GameStatus::get()->prison = new PrisonMap();

	g_shift_change_img = MOMOS::SpriteFromFile("data/shift.png");
	g_alarm_mode_img = MOMOS::SpriteFromFile("data/alarm.png");

	GameStatus::get()->map = new CostMap();
	GameStatus::get()->map->Load("data/map_03_60x44_bw.bmp", "data/map_03_960x704_layoutAB.bmp");
	GameStatus::get()->pathfinder_ = new Pathfinder();
	g_speed_controls.Initialize();

	//Sprinkle the loading area with crates
	PrisonMap* prison = GameStatus::get()->prison;
	for (int i = 0; i < 200; i++) {
		Crate* crate = new Crate();
		crate->pos_ = GameStatus::get()->map->MapToScreenCoords(prison->getRandomPointInRoom(prison->loading_area_));
		GameStatus::get()->crates_.push_back(crate);
	}

	//Close doors
	GameStatus::get()->prison->doors_[0]->is_open_ = false;
	GameStatus::get()->prison->doors_[1]->is_open_ = false;

	MOMOS::DrawSetTextFont("data/medieval.ttf");    
	MOMOS::WindowSetMouseVisibility(true);
	MOMOS::DrawSetFillColor(200, 50, 100, 255);

	// 40ms per frame
	float m_iTimeStep = 16.0f;
	double CurrentTime = MOMOS::Time();

	checkGameStarted();

	GameStatus::get()->working_shift_ = 0;
	GameStatus::get()->working_shift_time_end = GameStatus::get()->game_time + 5000.0;

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