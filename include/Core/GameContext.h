//------------------------------------------------------------------------------
// File: GameContext.h
// Purpose: Aggregates game services for dependency injection
//------------------------------------------------------------------------------
#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

class GameTimeService;
class MapService;
class PathfindingService;
class GameStateService;

/// Context object that aggregates game services for dependency injection
struct GameContext {
	GameTimeService* time;
	MapService* map;
	PathfindingService* pathfinding;
	GameStateService* state;
	
	GameContext()
		: time(nullptr)
		, map(nullptr)
		, pathfinding(nullptr)
		, state(nullptr)
	{
	}
	
	GameContext(GameTimeService* t, MapService* m, PathfindingService* p, GameStateService* s)
		: time(t)
		, map(m)
		, pathfinding(p)
		, state(s)
	{
	}
};

#endif // GAME_CONTEXT_H

