#include "../../include/ecs/PrisonerMovementUtils.h"

#include "../../include/Agents/Pathfinder.h"
#include "../../include/GameStatus.h"

#include <cmath>
#include <vector>


namespace {

::MOMOS::Vec2 SnapToWalkable(CostMap* map, ::MOMOS::Vec2 coords, const ::MOMOS::Vec2& dest) {
	if (!map) {
		return coords;
	}

	while (true) {
		Cell* cell = map->getCellAt(static_cast<int>(coords.x), static_cast<int>(coords.y));
		if (cell == nullptr || cell->is_walkable_) {
			break;
		}
		if (dest.x > coords.x) {
			coords.x += 1.0f;
		}
		if (dest.y > coords.y) {
			coords.y += 1.0f;
		}
	}

	return coords;
}

void PopulateDeterministicSteps(ECS::MovementComponent& movement, CostMap* map, const std::vector<::MOMOS::Vec2>& path) {
	movement.deterministic_steps.clear();
	if (!map) {
		return;
	}

	for (const auto& point : path) {
		movement.deterministic_steps.push_back(map->MapToScreenCoords(point));
	}

	movement.deterministic_step_index = 0;
}

void PopulateDeterministicSteps(ECS::MovementComponent& movement, CostMap* map, const std::vector<::MOMOS::Vec2>& path, const ::MOMOS::Vec2& extra) {
	PopulateDeterministicSteps(movement, map, path);
	if (map) {
		movement.deterministic_steps.push_back(map->MapToScreenCoords(extra));
	}
	movement.deterministic_step_index = 0;
}

} // namespace

namespace PrisonerECS {
namespace MovementUtils {

void ClearMovement(ECS::Registry& registry, ECS::Entity entity) {
	auto& movement = registry.GetComponent<ECS::MovementComponent>(entity);
	auto& transform = registry.GetComponent<ECS::TransformComponent>(entity);

	movement.deterministic_steps.clear();
	movement.deterministic_step_index = 0;
	movement.path_set = false;
	movement.movement_finished = false;
	movement.movement_path = nullptr;
	if (movement.path_command) {
		delete movement.path_command;
		movement.path_command = nullptr;
	}

	transform.direction = { 0.0f, 0.0f };
	movement.door_route_set = false;
	movement.escape_route_set = false;
}

bool TryFinalizePath(ECS::Registry& registry, ECS::Entity entity) {
	auto& movement = registry.GetComponent<ECS::MovementComponent>(entity);
	if (!movement.path_command || movement.path_command->pending_) {
		return false;
	}

	auto* status = GameStatus::get();
	if (!status || !status->map) {
		return false;
	}

	PopulateDeterministicSteps(movement, status->map, movement.path_command->path_->path_);
	movement.path_set = true;
	movement.movement_finished = false;
	movement.movement_path = movement.path_command->path_;
	delete movement.path_command;
	movement.path_command = nullptr;
	return true;
}

bool RequestPathTo(ECS::Registry& registry, ECS::Entity entity, const ::MOMOS::Vec2& destination) {
	auto* status = GameStatus::get();
	if (!status || !status->map || !status->pathfinder_) {
		return false;
	}

	auto& movement = registry.GetComponent<ECS::MovementComponent>(entity);
	auto& transform = registry.GetComponent<ECS::TransformComponent>(entity);
	auto* map = status->map;

	if (movement.path_command) {
		delete movement.path_command;
		movement.path_command = nullptr;
	}

	movement.movement_path = nullptr;
	auto start = map->ScreenToMapCoords(transform.position);
	auto target = map->ScreenToMapCoords(destination);
	auto snapped = SnapToWalkable(map, start, target);

	transform.position = map->MapToScreenCoords(snapped);

	auto* command = new PathCommand();
	command->start = snapped;
	command->end = target;
	command->calculated = false;
	command->pending_ = true;
	status->pathfinder_->search(command);
	movement.path_command = command;
	return true;
}

bool BuildRoomWaypointPath(ECS::Registry& registry, ECS::Entity entity, const Room& room, PrisonMap* prison, CostMap* map) {
	if (!prison || !map) {
		return false;
	}

	auto& movement = registry.GetComponent<ECS::MovementComponent>(entity);
	auto& transform = registry.GetComponent<ECS::TransformComponent>(entity);

	auto* current_room = prison->getRoomAt(map->ScreenToMapCoords(transform.position));
	std::vector<::MOMOS::Vec2> waypoint_path;

	if (current_room != nullptr) {
		if (current_room->id_ == room.id_) {
			ClearMovement(registry, entity);
			waypoint_path.push_back(prison->getRandomPointInRoom(room));
		} else {
			waypoint_path = prison->getPathToRoom(current_room, &room);
		}
	}

	if (waypoint_path.empty()) {
		return false;
	}

	PopulateDeterministicSteps(movement, map, waypoint_path, prison->getRandomPointInRoom(room));
	movement.path_set = true;
	movement.movement_finished = false;
	movement.movement_path = nullptr;
	return true;
}

void SetDoorRouteActive(ECS::Registry& registry, ECS::Entity entity, bool active) {
	registry.GetComponent<ECS::MovementComponent>(entity).door_route_set = active;
}

void SetEscapeRouteActive(ECS::Registry& registry, ECS::Entity entity, bool active) {
	registry.GetComponent<ECS::MovementComponent>(entity).escape_route_set = active;
}

void CycleDoorTarget(ECS::Registry& registry, ECS::Entity entity, int totalDoors) {
	if (totalDoors <= 0) {
		return;
	}
	auto& movement = registry.GetComponent<ECS::MovementComponent>(entity);
	movement.current_target_door = (movement.current_target_door + 1) % totalDoors;
}

void SetDoorTarget(ECS::Registry& registry, ECS::Entity entity, int target) {
	registry.GetComponent<ECS::MovementComponent>(entity).current_target_door = target;
}

} // namespace MovementUtils
} // namespace PrisonerECS

