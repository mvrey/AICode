//------------------------------------------------------------------------------
// File: GameWorldInputHandler.h
// Purpose: Handles input for game world interactions (pawn selection, cell clicking)
//------------------------------------------------------------------------------
#ifndef GAME_WORLD_INPUT_HANDLER_H
#define GAME_WORLD_INPUT_HANDLER_H

#include "IInputHandler.h"

class MapService;
class InfoPanel;

/// Handles input for game world interactions
class GameWorldInputHandler : public IInputHandler {
public:
	GameWorldInputHandler(MapService* map_service, InfoPanel* info_panel);
	
	bool HandleInput() override;
	int GetPriority() const override { return 50; } // Medium priority - after UI

private:
	MapService* map_service_;
	InfoPanel* info_panel_;
};

#endif // GAME_WORLD_INPUT_HANDLER_H

