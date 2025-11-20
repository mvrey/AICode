/*
* File:   GameStatus.h
* Author: Marcos Vazquez
*
* Created on February 14, 2017, 10:08 AM
*
* A class that holds info about program status and frame input
*/

#ifndef GAMESTATUS_H
#define GAMESTATUS_H

#include "config.h"
#include "PrisonMap.h"
#include "Crate.h"
#include "Pathfinding/cost_map.h"

class Pathfinder;

//forward declarations for agent cache
class Guard;
class Prisoner;
class Soldier;

class GameStatus {
public:
	static GameStatus* get();

	static GameStatus* instance_;

	/// GENERAL STATUS ///
	//In-game time
	double game_time;
	float simulation_speed_;
	//----------------///

	/// AGENTS MANAGEMENT ///
	//Agent amounts to be created
	bool g_agents_created = false;

	std::vector<Guard*> guards_;
	std::vector<Prisoner*> prisoners_;
	std::vector<Soldier*> soldiers_;
	//----------------///

	/// PATHFINDING ///
	PrisonMap* prison;
	CostMap* map;
	Pathfinder* pathfinder_;
	//----------------///


	/*********************
	 * PRISON STATUS INFO *
	 *********************/
	std::vector<Crate*> crates_;
	unsigned int first_available_crate_index = 0;

	double working_shift_time_end;
	short working_shift_ = 0;

	double alarm_mode_time_end_;
	bool alarm_mode_ = false;


protected:
	GameStatus();
	GameStatus(const GameStatus& orig);
	virtual ~GameStatus();

private:
	
  
};

#endif /* GAMESTATUS_H */