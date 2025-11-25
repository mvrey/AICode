/*
* File:   GameStatus.cc
* Author: Marcos Vazquez
*
* Created on February 14, 2017, 10:08 AM
*/

#include "../include/GameStatus.h"
#include "../include/Agents/Pathfinder.h"

GameStatus* GameStatus::instance_ = nullptr;

GameStatus::GameStatus() {
	game_time = 0;
	simulation_speed_ = 1.0f;
	prisoners_created = false;
}

GameStatus::GameStatus(const GameStatus& orig) {}
GameStatus::~GameStatus() {}

GameStatus* GameStatus::get() {
	if (instance_ == nullptr) {
		instance_ = new GameStatus();
	}

	return instance_;
}