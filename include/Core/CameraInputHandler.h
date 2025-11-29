//------------------------------------------------------------------------------
// File: CameraInputHandler.h
// Purpose: Handles camera input
//------------------------------------------------------------------------------
#ifndef CAMERA_INPUT_HANDLER_H
#define CAMERA_INPUT_HANDLER_H

#include "IInputHandler.h"

/// Handles camera input
class CameraInputHandler : public IInputHandler {
public:
	CameraInputHandler();
	
	bool HandleInput() override;
	int GetPriority() const override { return 10; } // Low priority - last

private:
	double last_input_time_;
};

#endif // CAMERA_INPUT_HANDLER_H

