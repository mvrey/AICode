//------------------------------------------------------------------------------
// File: CameraInputHandler.cc
// Purpose: Implementation of CameraInputHandler
//------------------------------------------------------------------------------
#include "../../include/Core/CameraInputHandler.h"
#include "../../include/Camera.h"
#include <MOMOS/time.h>

CameraInputHandler::CameraInputHandler()
	: last_input_time_(0.0)
{
}

bool CameraInputHandler::HandleInput() {
	double current_time = MOMOS::Time();
	if (last_input_time_ == 0.0) {
		last_input_time_ = current_time;
	}
	
	float delta_seconds = static_cast<float>(current_time - last_input_time_);
	if (delta_seconds < 0.0f) {
		delta_seconds = 0.0f;
	}
	last_input_time_ = current_time;
	
	Camera::HandleInput(delta_seconds);
	Camera::UpdateFollow(delta_seconds);
	
	return false; // Camera input doesn't consume clicks, always allow other handlers
}

