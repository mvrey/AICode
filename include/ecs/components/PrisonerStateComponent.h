//------------------------------------------------------------------------------
// File: PrisonerStateComponent.h
// Purpose: Declares the data that captures a prisoner's AI status, allowing the
//          ECS logic to track behavior, shift timing, and ownership without the
//          legacy AgentMind scaffolding.
//------------------------------------------------------------------------------
#ifndef ECS_PRISONER_STATE_COMPONENT_H
#define ECS_PRISONER_STATE_COMPONENT_H

#include "../../config.h"

class Crate;
class Agent;
class Prisoner;

namespace ECS {

// Represents high-level prisoner AI state such as current job, timers, and
// references to controlled resources so systems can reason deterministically.
struct PrisonerStateComponent {
	PrisonerStatus status = kIdle;
	short working_shift = 0;
	double time_end_status = 0.0;

	Crate* carried_crate = nullptr;
	float original_speed = 0.0f;

	Prisoner* pursuit_target = nullptr;
	Prisoner* owner = nullptr;
};

} // namespace ECS

#endif // ECS_PRISONER_STATE_COMPONENT_H

