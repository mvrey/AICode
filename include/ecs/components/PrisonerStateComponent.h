//------------------------------------------------------------------------------
// File: PrisonerStateComponent.h
// Purpose: Declares the data that captures a prisoner's AI status so ECS logic
//          can track behavior, shift timing, and carried crates.
//------------------------------------------------------------------------------
#ifndef ECS_PRISONER_STATE_COMPONENT_H
#define ECS_PRISONER_STATE_COMPONENT_H

#include "../../config.h"

class Crate;

namespace ECS {

struct PrisonerStateComponent {
	PrisonerStatus status = kIdle;
	short working_shift = 0;
	double time_end_status = 0.0;

	Crate* carried_crate = nullptr;
	float original_speed = 0.0f;

	::MOMOS::Vec2 wander_target{ 0.0f, 0.0f };
	bool has_wander_target = false;
};

} // namespace ECS

#endif // ECS_PRISONER_STATE_COMPONENT_H

