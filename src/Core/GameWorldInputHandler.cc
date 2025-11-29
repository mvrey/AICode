//------------------------------------------------------------------------------
// File: GameWorldInputHandler.cc
// Purpose: Implementation of GameWorldInputHandler
//------------------------------------------------------------------------------
#include "../../include/Core/GameWorldInputHandler.h"
#include "../../include/Core/MapService.h"
#include "../../include/UI/InfoPanel.h"
#include "../../include/ecs/PawnSelection.h"
#include "../../include/Map/Map.h"
#include <MOMOS/input.h>

GameWorldInputHandler::GameWorldInputHandler(MapService* map_service, InfoPanel* info_panel)
	: map_service_(map_service)
	, info_panel_(info_panel)
{
}

bool GameWorldInputHandler::HandleInput() {
	if (!MOMOS::MouseButtonDown(1)) {
		return false;
	}
	
	if (!map_service_ || !map_service_->GetMap() || !info_panel_) {
		return false;
	}
	
	float mx = static_cast<float>(MOMOS::MousePositionX());
	float my = static_cast<float>(MOMOS::MousePositionY());
	
	::MOMOS::Vec2 mouse_screen = { mx, my };
	
	bool pawn_clicked = PawnSelection::HandleClick();
	if (pawn_clicked) {
		// Clear cell selection when pawn is clicked
		map_service_->GetMap()->ClearCellSelection();
		return true; // Input consumed
	} else {
		// Clear pawn selection when clicking elsewhere
		PawnSelection::ClearSelection();
		info_panel_->SetSelectedPawn(ECS::Entity());
		
		// Try to click on a cell
		if (map_service_->GetMap()->HandleCellClick(mouse_screen)) {
			return true; // Input consumed
		}
	}
	
	return false; // Input not consumed
}

