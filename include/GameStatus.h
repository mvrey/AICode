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
#include "Pathfinding/cost_map.h"
class Pathfinder;

//forward declarations for agent cache
class Guard;
class Pawn;
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

	bool pawns_created = false;

	//----------------///

	/// PATHFINDING ///
	PrisonMap* prison;
	CostMap* map;
	Pathfinder* pathfinder_;
	//----------------///


	/*********************
	 * PRISON STATUS INFO *
	 *********************/
	unsigned int first_available_crate_index = 0;

	double working_shift_time_end;
	short working_shift_ = 0;

protected:
	GameStatus();
	GameStatus(const GameStatus& orig);
	virtual ~GameStatus();

private:
	
  
};

#endif /* GAMESTATUS_H */