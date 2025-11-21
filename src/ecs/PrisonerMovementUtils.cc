#include "../../include/ecs/PrisonerMovementUtils.h"

#include <algorithm>
#include <cmath>

#include "../../include/Agents/Prisoner.h"
#include "../../include/Agents/Pathfinder.h"
#include "../../include/GameStatus.h"
#include "../../include/Pathfinding/astar.h"
#include "../../include/Pathfinding/cost_map.h"

namespace {

void MirrorMovementToLegacy(Prisoner& prisoner, const ECS::MovementComponent& movement) {
	prisoner.deterministic_steps_ = movement.deterministic_steps;
	prisoner.deterministic_step_num_ = movement.deterministic_step_index;
	prisoner.getBody()->path_set_ = movement.path_set;
	prisoner.movement_path_ = movement.movement_path;
	prisoner.path_cmd_ = movement.path_command;
	prisoner.last_movement_update_ = movement.last_movement_update;
	if (prisoner.mind_) {
		prisoner.mind_->movement_finished_ = movement.movement_finished;
	}
}

void ApplyDirection(Prisoner& prisoner, const ::MOMOS::Vec2& direction) {
	auto& transform = prisoner.GetTransformComponent();
	transform.direction = direction;
	prisoner.getBody()->direction_ = direction;
}

::MOMOS::Vec2 Normalize(const ::MOMOS::Vec2& from, const ::MOMOS::Vec2& to) {
	float dx = to.x - from.x;
	float dy = to.y - from.y;
	float dist = std::sqrt(dx * dx + dy * dy);
	if (dist <= 0.0001f) {
		return { 0.0f, 0.0f };
	}
	return { dx / dist, dy / dist };
}

} // namespace

namespace PrisonerECS {
namespace MovementUtils {

bool SetPathTo(Prisoner& prisoner, const ::MOMOS::Vec2& destination) {
	auto& movement = prisoner.GetMovementComponent();
	auto& transform = prisoner.GetTransformComponent();

	CostMap* map = GameStatus::get()->map;
	if (!map) {
		return false;
	}

	::MOMOS::Vec2 current_map_pos = map->ScreenToMapCoords(transform.position);
	::MOMOS::Vec2 dest_map_pos = map->ScreenToMapCoords(destination);
	bool displaced = false;

	while (true) {
		Cell* cell = map->getCellAt(static_cast<int>(current_map_pos.x), static_cast<int>(current_map_pos.y));
		if (cell == nullptr || cell->is_walkable_) {
			break;
		}
		if (dest_map_pos.x > current_map_pos.x) current_map_pos.x++;
		if (dest_map_pos.y > current_map_pos.y) current_map_pos.y++;
		displaced = true;
	}

	if (displaced) {
		::MOMOS::Vec2 screen_pos = map->MapToScreenCoords(current_map_pos);
		transform.position = screen_pos;
		prisoner.getBody()->pos_ = screen_pos;
	}

	if (!movement.path_set) {
		if (movement.path_command == nullptr) {
			auto* command = new PathCommand();
			command->start = current_map_pos;
			command->end = dest_map_pos;
			command->calculated = false;
			command->pending_ = true;
			GameStatus::get()->pathfinder_->search(command);
			movement.path_command = command;
			prisoner.path_cmd_ = command;
		}

		if (movement.path_command && movement.path_command->pending_) {
			MirrorMovementToLegacy(prisoner, movement);
			return false;
		}

		if (movement.path_command && movement.path_command->calculated) {
			movement.movement_path = movement.path_command->path_;
			prisoner.movement_path_ = movement.movement_path;
			delete movement.path_command;
			movement.path_command = nullptr;
			prisoner.path_cmd_ = nullptr;
		}

		if (movement.movement_path && movement.movement_path->path_.size() > 1) {
			movement.path_set = true;
			movement.deterministic_steps.clear();
			for (const auto& point : movement.movement_path->path_) {
				movement.deterministic_steps.push_back(map->MapToScreenCoords(point));
			}
			movement.deterministic_step_index = 0;
			movement.movement_finished = false;
			if (prisoner.mind_) {
				prisoner.mind_->movement_finished_ = false;
			}
		}
	}

	MirrorMovementToLegacy(prisoner, movement);
	return movement.path_set;
}

bool MoveFollowingPath(Prisoner& prisoner) {
	auto& movement = prisoner.GetMovementComponent();
	auto& transform = prisoner.GetTransformComponent();

	if (!prisoner.mind_) {
		return true;
	}

	if (movement.movement_finished || !movement.path_set || movement.deterministic_steps.empty()) {
		if (movement.path_set && movement.deterministic_steps.empty()) {
			movement.path_set = false;
		}
		MirrorMovementToLegacy(prisoner, movement);
		return true;
	}

	CostMap* map = GameStatus::get()->map;
	if (!map) {
		return true;
	}

	::MOMOS::Vec2 target = movement.deterministic_steps[movement.deterministic_step_index];
	::MOMOS::Vec2 target_cell = map->ScreenToMapCoords(target);
	Cell* cell = map->getCellAt(static_cast<int>(target_cell.x), static_cast<int>(target_cell.y));

	if (!cell || !cell->is_walkable_) {
		ClearMovement(prisoner);
		MirrorMovementToLegacy(prisoner, movement);
		return true;
	}

	movement.last_movement_update = GameStatus::get()->game_time;

	float dx = target.x - transform.position.x;
	float dy = target.y - transform.position.y;
	float dist = std::sqrt(dx * dx + dy * dy);

	if (dist < 5.0f) {
		if (movement.deterministic_step_index + 1 >= movement.deterministic_steps.size()) {
			movement.movement_finished = true;
			if (prisoner.mind_) {
				prisoner.mind_->movement_finished_ = true;
			}
		} else {
			movement.deterministic_step_index =
				(movement.deterministic_step_index + 1) % movement.deterministic_steps.size();
			target = movement.deterministic_steps[movement.deterministic_step_index];
			dx = target.x - transform.position.x;
			dy = target.y - transform.position.y;
			dist = std::sqrt(dx * dx + dy * dy);
			movement.movement_finished = false;
			if (prisoner.mind_) {
				prisoner.mind_->movement_finished_ = false;
			}
		}
	}

	if (!movement.movement_finished) {
		if (dist > 0.0001f) {
			::MOMOS::Vec2 direction{ dx / dist, dy / dist };
			ApplyDirection(prisoner, direction);
		}
	} else {
		ApplyDirection(prisoner, { 0.0f, 0.0f });
	}

	MirrorMovementToLegacy(prisoner, movement);
	return movement.movement_finished;
}

void ClearMovement(Prisoner& prisoner) {
	auto& movement = prisoner.GetMovementComponent();
	auto& transform = prisoner.GetTransformComponent();

	movement.deterministic_steps.clear();
	movement.deterministic_step_index = 0;
	movement.path_set = false;
	movement.last_movement_update = 0.0;

	movement.movement_finished = false;
	if (prisoner.mind_) {
		prisoner.mind_->movement_finished_ = false;
	}

	::MOMOS::Vec2 zero_dir{ 0.0f, 0.0f };
	transform.direction = zero_dir;
	if (prisoner.getBody()) {
		prisoner.getBody()->stop();
		prisoner.getBody()->direction_ = zero_dir;
		prisoner.getBody()->path_set_ = false;
	}

	prisoner.deterministic_steps_.clear();
	prisoner.deterministic_step_num_ = 0;

	MirrorMovementToLegacy(prisoner, movement);
}

void SetDoorRouteActive(Prisoner& prisoner, bool active) {
	auto& movement = prisoner.GetMovementComponent();
	movement.door_route_set = active;
}

void CycleDoorTarget(Prisoner& prisoner, int totalDoors) {
	auto& movement = prisoner.GetMovementComponent();
	if (totalDoors <= 0) {
		return;
	}
	movement.current_target_door = (movement.current_target_door + 1) % totalDoors;
}

void SetDoorTarget(Prisoner& prisoner, int doorIndex) {
	auto& movement = prisoner.GetMovementComponent();
	movement.current_target_door = doorIndex;
}

void SetEscapeRouteActive(Prisoner& prisoner, bool active) {
	auto& movement = prisoner.GetMovementComponent();
	movement.escape_route_set = active;
}

} // namespace MovementUtils
} // namespace PrisonerECS

