#ifndef ECS_PRISONER_STATE_COMPONENT_H
#define ECS_PRISONER_STATE_COMPONENT_H

#include "../../config.h"

class Crate;
class Agent;
class Prisoner;

namespace ECS {

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

