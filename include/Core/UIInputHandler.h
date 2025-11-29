//------------------------------------------------------------------------------
// File: UIInputHandler.h
// Purpose: Handles input for UI controls (VSync, SpeedControls, InfoPanel)
//------------------------------------------------------------------------------
#ifndef UI_INPUT_HANDLER_H
#define UI_INPUT_HANDLER_H

#include "IInputHandler.h"

class SimulationSpeedControls;
class VSyncToggle;
class InfoPanel;

/// Handles input for all UI controls
class UIInputHandler : public IInputHandler {
public:
	UIInputHandler(SimulationSpeedControls* speed_controls, 
	               VSyncToggle* vsync_toggle,
	               InfoPanel* info_panel);
	
	bool HandleInput() override;
	int GetPriority() const override { return 100; } // High priority - UI first

private:
	SimulationSpeedControls* speed_controls_;
	VSyncToggle* vsync_toggle_;
	InfoPanel* info_panel_;
};

#endif // UI_INPUT_HANDLER_H

