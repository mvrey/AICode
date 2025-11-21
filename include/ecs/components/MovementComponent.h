#ifndef ECS_MOVEMENT_COMPONENT_H
#define ECS_MOVEMENT_COMPONENT_H

#include <vector>
#include <MOMOS/math.h>

class Path;
class PathCommand;

namespace ECS {

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
};

} // namespace ECS

#endif // ECS_MOVEMENT_COMPONENT_H

