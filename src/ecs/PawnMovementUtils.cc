#include "../../include/ecs/PawnMovementUtils.h"

#include "../../include/Agents/Pathfinder.h"
#include "../../include/GameStatus.h"

#include <cmath>
#include <vector>


namespace {

::MOMOS::Vec2 SnapToWalkable(CostMap* map, ::MOMOS::Vec2 coords, const ::MOMOS::Vec2& dest) {
	if (!map) {
		return coords;
	}

	int width = map->getWidth();
	int height = map->getHeight();
	if (width <= 0 || height <= 0) {
		return coords;
	}

	const int max_steps = width * height;
	int step_count = 0;
	::MOMOS::Vec2 current = coords;

	while (step_count < max_steps) {
		MapCell* cell = map->getCellAt(static_cast<int>(current.x), static_cast<int>(current.y));
		if (cell == nullptr || cell->isWalkable()) {
			coords = current;
			return coords;
		}

		float prev_x = current.x;
		float prev_y = current.y;

		int delta_x = (dest.x > current.x) ? 1 : (dest.x < current.x) ? -1 : 0;
		int delta_y = (dest.y > current.y) ? 1 : (dest.y < current.y) ? -1 : 0;

		if (delta_x == 0 && delta_y == 0) {
			break;
		}

		current.x += delta_x;
		current.y += delta_y;

		if (current.x < 0.0f) {
			current.x = 0.0f;
		} else if (current.x > static_cast<float>(width - 1)) {
			current.x = static_cast<float>(width - 1);
		}

		if (current.y < 0.0f) {
			current.y = 0.0f;
		} else if (current.y > static_cast<float>(height - 1)) {
			current.y = static_cast<float>(height - 1);
		}

		if (current.x == prev_x && current.y == prev_y) {
			break;
		}

		++step_count;
	}

	return current;
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

namespace PawnECS {
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

PathFinalizationResult TryFinalizePath(ECS::Registry& registry, ECS::Entity entity) {
	auto& movement = registry.GetComponent<ECS::MovementComponent>(entity);
	if (!movement.path_command || movement.path_command->pending_ || movement.path_command->path_ == nullptr) {
		return PathFinalizationResult::kNotReady;
	}

	auto* path_command = movement.path_command;
	auto* path = path_command->path_;
	if (path == nullptr || path->path_.empty()) {
		delete path;
		delete path_command;
		movement.path_command = nullptr;
		movement.movement_path = nullptr;
		return PathFinalizationResult::kFailure;
	}

	const auto& points = path->path_;
	const auto& start = path_command->start;
	if (points.front().x != start.x || points.front().y != start.y) {
		delete path;
		delete path_command;
		movement.path_command = nullptr;
		movement.movement_path = nullptr;
		return PathFinalizationResult::kFailure;
	}

	auto* status = GameStatus::get();
	if (!status || !status->map) {
		delete path;
		delete path_command;
		movement.path_command = nullptr;
		movement.movement_path = nullptr;
		return PathFinalizationResult::kNotReady;
	}

	PopulateDeterministicSteps(movement, status->map, points);
	movement.path_set = true;
	movement.movement_finished = false;
	movement.movement_path = path;
	delete movement.path_command;
	movement.path_command = nullptr;
	return PathFinalizationResult::kSuccess;
}

bool RequestPathTo(ECS::Registry& registry, ECS::Entity entity, const ::MOMOS::Vec2& destination) {
	auto* status = GameStatus::get();
	if (!status || !status->map || !status->pathfinder_) {
		return false;
	}

	auto& movement = registry.GetComponent<ECS::MovementComponent>(entity);
	auto& transform = registry.GetComponent<ECS::TransformComponent>(entity);
	auto* map = status->map;

	auto start = map->ScreenToMapCoords(transform.position);
	auto target = map->ScreenToMapCoords(destination);
	auto snapped = SnapToWalkable(map, start, target);

	transform.position = map->MapToScreenCoords(snapped);

	if (movement.path_command) {
		if (movement.path_command->pending_) {
			movement.path_command->start = snapped;
			movement.path_command->end = target;
			return true;
		}
		delete movement.path_command;
		movement.path_command = nullptr;
	}

	movement.movement_path = nullptr;

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
			waypoint_path = prison->getPathToRoom(current_room, const_cast<Room*>(&room));
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
} // namespace PawnECS

