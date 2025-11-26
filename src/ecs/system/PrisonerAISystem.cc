//------------------------------------------------------------------------------
// File: PrisonerAISystem.cc
// Purpose: Implements the prisoner AI state machine using ECS components,
//          replacing the legacy AgentMind logic.
//------------------------------------------------------------------------------
#include "../../../include/ecs/system/PrisonerAISystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/PrisonerComponents.h"
#include "../../../include/ecs/PrisonerMovementUtils.h"
#include "../../../include/GameStatus.h"
#include "../../../include/PrisonMap.h"
#include "../../../include/Pathfinding/cost_map.h"
#include "../../../include/config.h"

#include <MOMOS/math.h>
#include <cstdlib>

namespace {

namespace MovementUtils = PrisonerECS::MovementUtils;

bool SampleWalkableDestination(CostMap* map, ::MOMOS::Vec2& out) {
	if (!map) {
		return false;
	}

	int width = map->getWidth();
	int height = map->getHeight();
	if (width <= 0 || height <= 0) {
		return false;
	}

	for (int attempts = 0; attempts < 1024; ++attempts) {
		int x = rand() % width;
		int y = rand() % height;
		Cell* cell = map->getCellAt(x, y);
		if (cell && cell->is_walkable_) {
			out = map->MapToScreenCoords({ static_cast<float>(x), static_cast<float>(y) });
			return true;
		}
	}

	return false;
}

} // namespace

namespace ECS {

void PrisonerAISystem::Update(Registry& registry, double /*delta_time*/) {
	auto* status = GameStatus::get();
	if (!status || !status->map) {
		return;
	}

	CostMap* map = status->map;

	registry.ForEach<PrisonerStateComponent>([&](Entity entity, PrisonerStateComponent& state) {
		if (!registry.HasComponent<MovementComponent>(entity) ||
			!registry.HasComponent<TransformComponent>(entity)) {
			return;
		}

		auto& movement = registry.GetComponent<MovementComponent>(entity);
		auto& transform = registry.GetComponent<TransformComponent>(entity);

		auto ensureTarget = [&]() -> bool {
			if (state.has_wander_target) {
				return true;
			}
			::MOMOS::Vec2 candidate{};
			if (!SampleWalkableDestination(map, candidate)) {
				return false;
			}
			state.wander_target = candidate;
			state.has_wander_target = true;
			MovementUtils::ClearMovement(registry, entity);
			return true;
		};

		if (movement.movement_finished) {
			state.has_wander_target = false;
			MovementUtils::ClearMovement(registry, entity);
		}

		if (!ensureTarget()) {
			return;
		}

		auto finalize_result = MovementUtils::TryFinalizePath(registry, entity);

		if (finalize_result == MovementUtils::PathFinalizationResult::kFailure) {
			state.has_wander_target = false;
			MovementUtils::ClearMovement(registry, entity);
			if (!ensureTarget()) {
				return;
			}
			MovementUtils::RequestPathTo(registry, entity, state.wander_target);
			return;
		}

		if (!movement.path_set) {
			MovementUtils::RequestPathTo(registry, entity, state.wander_target);
			return;
		}

		if (movement.movement_finished) {
			state.has_wander_target = false;
			MovementUtils::ClearMovement(registry, entity);
			if (ensureTarget()) {
				MovementUtils::RequestPathTo(registry, entity, state.wander_target);
			}
		}
	});
}

} // namespace ECS

