#include <stdio.h>
#include <vector>
#include <ctime>
#include <thread>
#include <algorithm>

#include "../include/GameStatus.h"
#include "../include/PrisonMap.h"
#include "../include/config.h"
#include "../include/Pathfinding/astar.h"

#include "../include/Agents/Soldier.h"
#include "../include/Agents/Guard.h"
#include "../include/Agents/Prisoner.h"
#include "../include/Agents/Pathfinder.h"


// Global vars
double g_shift_change_time_end = 0;
MOMOS::SpriteHandle g_shift_change_img;
MOMOS::SpriteHandle g_alarm_mode_img;


/// SPEED CONTROLS CODE STARTS HERE ///
struct SpeedControlOption {
	const char* label;
	double multiplier;
};

const SpeedControlOption kSpeedControlOptions[] = {
	{ "Paused", 0.0 },
	{ "1x", 1.0 },
	{ "2x", 2.0 },
	{ "3x", 3.0 },
	{ "4x", 4.0 },
	{ "5x", 5.0 },
};

const int kSpeedControlCount = sizeof(kSpeedControlOptions) / sizeof(SpeedControlOption);

int g_speed_index = 1;
int g_last_nonzero_speed_index = 1;
int g_speed_button_hover = -1;

const float kSpeedUIRightPadding = 16.0f;
const float kSpeedUITopPadding = 16.0f;
const float kSpeedUIButtonWidth = 32.0f;
const float kSpeedUIButtonHeight = 26.0f;
const float kSpeedUIButtonSpacing = 4.0f;
const float kSpeedUITextSize = 16.0f;
const int kSpeedUIButtonCount = 3; // -, toggle, +

float SpeedControlsTotalWidth() {
	return kSpeedUIButtonCount * kSpeedUIButtonWidth + (kSpeedUIButtonCount - 1) * kSpeedUIButtonSpacing;
}

float SpeedControlsBaseX() {
	return Screen::width - kSpeedUIRightPadding - SpeedControlsTotalWidth();
}

float SpeedControlsButtonsY() {
	return kSpeedUITopPadding + kSpeedUITextSize + 6.0f;
}

float SpeedButtonX(int button_index) {
	return SpeedControlsBaseX() + button_index * (kSpeedUIButtonWidth + kSpeedUIButtonSpacing);
}

bool IsPointInsideSpeedButton(int button_index, float x, float y) {
	float bx = SpeedButtonX(button_index);
	float by = SpeedControlsButtonsY();
	return (x >= bx && x <= bx + kSpeedUIButtonWidth &&
		y >= by && y <= by + kSpeedUIButtonHeight);
}

int GetSpeedButtonIndexAt(float x, float y) {
	for (int i = 0; i < kSpeedUIButtonCount; ++i) {
		if (IsPointInsideSpeedButton(i, x, y)) {
			return i;
		}
	}
	return -1;
}

void ApplySpeedIndex(int new_index) {
	new_index = std::max(0, std::min(kSpeedControlCount - 1, new_index));
	g_speed_index = new_index;
	GameStatus::get()->simulation_speed_ = static_cast<float>(kSpeedControlOptions[new_index].multiplier);

	if (kSpeedControlOptions[new_index].multiplier > 0.0) {
		g_last_nonzero_speed_index = new_index;
	}
}

void IncreaseSimulationSpeed() {
	if (g_speed_index < kSpeedControlCount - 1) {
		ApplySpeedIndex(g_speed_index + 1);
	}
}

void DecreaseSimulationSpeed() {
	if (g_speed_index > 0) {
		ApplySpeedIndex(g_speed_index - 1);
	}
}

void ToggleSimulationSpeed() {
	if (kSpeedControlOptions[g_speed_index].multiplier == 0.0) {
		int target = (g_last_nonzero_speed_index > 0) ? g_last_nonzero_speed_index : 1;
		ApplySpeedIndex(target);
	} else {
		g_last_nonzero_speed_index = g_speed_index;
		ApplySpeedIndex(0);
	}
}

void HandleSimulationSpeedControls() {
	float mx = static_cast<float>(MOMOS::MousePositionX());
	float my = static_cast<float>(MOMOS::MousePositionY());
	int hovered = GetSpeedButtonIndexAt(mx, my);
	g_speed_button_hover = hovered;

	if (hovered != -1 && MOMOS::MouseButtonDown(1)) {
		switch (hovered) {
		case 0:
			DecreaseSimulationSpeed();
			break;
		case 1:
			ToggleSimulationSpeed();
			break;
		case 2:
			IncreaseSimulationSpeed();
			break;
		}
	}
}

void DrawSimulationSpeedControls() {
	const SpeedControlOption& current = kSpeedControlOptions[g_speed_index];
	char label_text[32];
	if (current.multiplier == 0.0) {
		snprintf(label_text, sizeof(label_text), "Speed: Paused");
	} else {
		snprintf(label_text, sizeof(label_text), "Speed: %s", current.label);
	}

	MOMOS::DrawSetFillColor(240, 240, 240, 255);
	MOMOS::DrawSetTextSize(kSpeedUITextSize);
	MOMOS::DrawText(SpeedControlsBaseX(), kSpeedUITopPadding + kSpeedUITextSize, label_text);

	const char* button_labels[kSpeedUIButtonCount];
	button_labels[0] = "-";
	button_labels[1] = (current.multiplier == 0.0) ? ">" : "||";
	button_labels[2] = "+";

	for (int i = 0; i < kSpeedUIButtonCount; ++i) {
		float bx = SpeedButtonX(i);
		float by = SpeedControlsButtonsY();

		bool disabled = (i == 0 && g_speed_index == 0) ||
			(i == 2 && g_speed_index >= kSpeedControlCount - 1 && current.multiplier != 0.0);

		unsigned char baseColor = disabled ? 40 : 65;
		unsigned char alpha = disabled ? 90 : 150;

		if (i == g_speed_button_hover && !disabled) {
			baseColor = 85;
			alpha = 200;
		}

		float points[10] = {
			bx, by,
			bx + kSpeedUIButtonWidth, by,
			bx + kSpeedUIButtonWidth, by + kSpeedUIButtonHeight,
			bx, by + kSpeedUIButtonHeight,
			bx, by
		};

		MOMOS::DrawSetFillColor(baseColor, baseColor, baseColor, alpha);
		MOMOS::DrawSolidPath(points, 5);
		MOMOS::DrawSetStrokeColor(220, 220, 220, 180);
		MOMOS::DrawPath(points, 5);

		MOMOS::DrawSetFillColor(240, 240, 240, disabled ? 120 : 255);
		MOMOS::DrawSetTextSize(14.0f);
		float text_x = bx + kSpeedUIButtonWidth / 2.0f - 6.0f;
		float text_y = by + kSpeedUIButtonHeight / 2.0f + 5.0f;
		MOMOS::DrawText(text_x, text_y, button_labels[i]);
	}
}

/// SPEED CONTROLS CODE ENDS HERE ///


/// Process user input
void Input() {
	HandleSimulationSpeedControls();
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

	DrawSimulationSpeedControls();

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

	//Init variables and locations for this specific map
	GameStatus::get()->prison = new PrisonMap();

	g_shift_change_img = MOMOS::SpriteFromFile("data/shift.png");
	g_alarm_mode_img = MOMOS::SpriteFromFile("data/alarm.png");

	GameStatus::get()->map = new CostMap();
	GameStatus::get()->map->Load("data/map_03_60x44_bw.bmp", "data/map_03_960x704_layoutAB.bmp");
	GameStatus::get()->pathfinder_ = new Pathfinder();
	GameStatus::get()->simulation_speed_ = static_cast<float>(kSpeedControlOptions[g_speed_index].multiplier);

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