//------------------------------------------------------------------------------
// File: PrisonerPathFollowSystem.cc
// Purpose: Validates queued path steps for each prisoner and steers transforms
//          toward their next waypoint.
//------------------------------------------------------------------------------
#include "../../../include/ecs/system/PrisonerPathFollowSystem.h"

#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/MovementComponent.h"
#include "../../../include/ecs/components/TransformComponent.h"
#include "../../../include/GameStatus.h"
#include "../../../include/Pathfinding/cost_map.h"

#include <MOMOS/math.h>
#include <cmath>

namespace {

constexpr float kArrivalThresholdSq = 25.0f; // 5px

// Resets path-following state when a route becomes invalid.
void ClearMovement(ECS::MovementComponent& movement, ECS::TransformComponent& transform) {
	movement.deterministic_steps.clear();
	movement.deterministic_step_index = 0;
	movement.path_set = false;
	movement.movement_finished = false;
	transform.direction = { 0.0f, 0.0f };
}

} // namespace

namespace ECS {

// Steps entities toward their current deterministic waypoint and reacts when
// cells become invalid or when the path is completed.
void PrisonerPathFollowSystem::Update(Registry& registry, double /*delta_time*/) {
	CostMap* map = GameStatus::get()->map;
	if (!map) {
		return;
	}

	registry.ForEach<MovementComponent>([&](Entity entity, MovementComponent& movement) {
		if (!registry.HasComponent<TransformComponent>(entity)) {
			return;
		}

		auto& transform = registry.GetComponent<TransformComponent>(entity);

		if (!movement.path_set || movement.deterministic_steps.empty()) {
			movement.movement_finished = false;
			return;
		}

		if (movement.deterministic_step_index >= movement.deterministic_steps.size()) {
			movement.deterministic_step_index =
				static_cast<unsigned int>(movement.deterministic_steps.size() - 1);
		}

		::MOMOS::Vec2 target = movement.deterministic_steps[movement.deterministic_step_index];
		::MOMOS::Vec2 cell_coords = map->ScreenToMapCoords(target);
		Cell* cell = map->getCellAt(static_cast<int>(cell_coords.x), static_cast<int>(cell_coords.y));
		if (!cell || !cell->is_walkable_) {
			ClearMovement(movement, transform);
			return;
		}

		movement.last_movement_update = GameStatus::get()->game_time;

		::MOMOS::Vec2 diff{ target.x - transform.position.x, target.y - transform.position.y };
		float dist_sq = diff.x * diff.x + diff.y * diff.y;

		if (dist_sq <= kArrivalThresholdSq) {
			if (movement.deterministic_step_index + 1 < movement.deterministic_steps.size()) {
				movement.deterministic_step_index++;
				target = movement.deterministic_steps[movement.deterministic_step_index];
				diff = { target.x - transform.position.x, target.y - transform.position.y };
				dist_sq = diff.x * diff.x + diff.y * diff.y;
				movement.movement_finished = false;
			} else {
				movement.path_set = false;
				movement.deterministic_steps.clear();
				movement.deterministic_step_index = 0;
				movement.movement_finished = true;
				transform.direction = { 0.0f, 0.0f };
				return;
			}
		} else {
			movement.movement_finished = false;
		}

		float dist = std::sqrt(dist_sq);
		if (dist > 0.0001f) {
			transform.direction = { diff.x / dist, diff.y / dist };
		} else {
			transform.direction = { 0.0f, 0.0f };
		}
	});
}

} // namespace ECS

