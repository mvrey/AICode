//------------------------------------------------------------------------------
// File: MovementComponent.h
// Purpose: Declares the data that tracks per-entity locomotion intent and
//          progress so systems can advance transforms without touching legacy
//          Agent state.
//------------------------------------------------------------------------------
#ifndef ECS_MOVEMENT_COMPONENT_H
#define ECS_MOVEMENT_COMPONENT_H

#include <vector>
#include <MOMOS/math.h>

class Path;
class PathCommand;

namespace ECS {

// Captures all path-following state for an entity, including the queued path,
// current progress, and bookkeeping for special routes like doors or escapes.
struct MovementComponent {
	float speed = 0.0f;
	double last_movement_update = 0.0;
	double movement_threshold = 0.0;

	std::vector<::MOMOS::Vec2> deterministic_steps;
	unsigned int deterministic_step_index = 0;

	bool path_set = false;
	Path* movement_path = nullptr;
	PathCommand* path_command = nullptr;

	bool door_route_set = false;
	bool escape_route_set = false;
	int current_target_door = 1;

	bool movement_finished = true;
};

} // namespace ECS

#endif // ECS_MOVEMENT_COMPONENT_H

