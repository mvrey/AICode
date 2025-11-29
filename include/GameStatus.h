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
class Map;
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
	Map* map;  // Changed from CostMap* to Map* in Phase 1 decoupling
	Pathfinder* pathfinder_;
	//----------------///

protected:
	GameStatus();
	GameStatus(const GameStatus& orig);
	virtual ~GameStatus();

private:
	
  
};

#endif /* GAMESTATUS_H */