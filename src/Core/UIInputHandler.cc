//------------------------------------------------------------------------------
// File: UIInputHandler.cc
// Purpose: Implementation of UIInputHandler
//------------------------------------------------------------------------------
#include "../../include/Core/UIInputHandler.h"
#include "../../include/UI/SimulationSpeedControls.h"
#include "../../include/UI/VSyncToggle.h"
#include "../../include/UI/InfoPanel.h"
#include <MOMOS/input.h>

UIInputHandler::UIInputHandler(SimulationSpeedControls* speed_controls,
                               VSyncToggle* vsync_toggle,
                               InfoPanel* info_panel)
	: speed_controls_(speed_controls)
	, vsync_toggle_(vsync_toggle)
	, info_panel_(info_panel)
{
}

bool UIInputHandler::HandleInput() {
	if (!speed_controls_ || !vsync_toggle_ || !info_panel_) {
		return false;
	}
	
	// Handle speed controls input
	speed_controls_->HandleInput();
	
	// Check for UI clicks
	if (MOMOS::MouseButtonDown(1)) {
		float mx = static_cast<float>(MOMOS::MousePositionX());
		float my = static_cast<float>(MOMOS::MousePositionY());
		
		// Check VSync toggle
		if (vsync_toggle_->HandleClick(mx, my)) {
			return true; // Input consumed
		}
		
		// Check speed controls
		if (speed_controls_->IsClickOnControls(mx, my)) {
			return true; // Input consumed
		}
		
		// Check InfoPanel
		if (info_panel_->HandleClick(mx, my)) {
			return true; // Input consumed
		}
	}
	
	return false; // Input not consumed, continue processing
}

