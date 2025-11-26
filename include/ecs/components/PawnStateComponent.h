//------------------------------------------------------------------------------
// File: PAWNStateComponent.h
// Purpose: Declares the data that captures a pawn's AI status so ECS logic can track behavior.
//------------------------------------------------------------------------------
#ifndef ECS_PAWN_STATE_COMPONENT_H
#define ECS_PAWN_STATE_COMPONENT_H

#include "../../config.h"
#include <string>

namespace ECS {

struct PAWNStateComponent {
	PAWNStatus status = kIdle;
	double time_end_status = 0.0;

	float original_speed = 0.0f;
	std::string name;

	::MOMOS::Vec2 wander_target{ 0.0f, 0.0f };
	bool has_wander_target = false;
};

} // namespace ECS

#endif // ECS_PAWN_STATE_COMPONENT_H

