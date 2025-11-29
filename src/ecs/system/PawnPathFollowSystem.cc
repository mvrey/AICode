//------------------------------------------------------------------------------
// File: PawnPathFollowSystem.cc
// Purpose: Validates queued path steps for each pawn and steers transforms
//          toward their next waypoint.
//------------------------------------------------------------------------------
#include "../../../include/ecs/system/PawnPathFollowSystem.h"

#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/MovementComponent.h"
#include "../../../include/ecs/components/TransformComponent.h"
#include "../../../include/Core/GameContext.h"
#include "../../../include/Core/MapService.h"
#include "../../../include/Core/GameTimeService.h"
#include "../../../include/Map/Map.h"

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
void PawnPathFollowSystem::Update(Registry& registry, double /*delta_time*/, const GameContext* context) {
	if (!context || !context->map || !context->map->GetMap()) {
		return;
	}
	
	Map* map = context->map->GetMap();

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
		MapCell* cell = map->getCellAt(static_cast<int>(cell_coords.x), static_cast<int>(cell_coords.y));
		if (!cell || !cell->isWalkable()) {
			ClearMovement(movement, transform);
			return;
		}

		if (context->time) {
			movement.last_movement_update = context->time->GetGameTime();
		}

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

