//------------------------------------------------------------------------------
// File: PawnAISystem.cc
// Purpose: Implements the pawn AI state machine using ECS components,
//          replacing the legacy AgentMind logic.
//------------------------------------------------------------------------------
#include "../../../include/ecs/system/PawnAISystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/PawnComponents.h"
#include "../../../include/ecs/PawnMovementUtils.h"
#include "../../../include/Core/GameContext.h"
#include "../../../include/Core/MapService.h"
#include "../../../include/Map/Map.h"
#include "../../../include/config.h"

#include <MOMOS/math.h>
#include <cstdlib>

namespace {

namespace MovementUtils = PawnECS::MovementUtils;

bool SampleWalkableDestination(Map* map, ::MOMOS::Vec2& out) {
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
		MapCell* cell = map->getCellAt(x, y);
		if (cell && cell->isWalkable()) {
			out = map->MapToScreenCoords({ static_cast<float>(x), static_cast<float>(y) });
			return true;
		}
	}

	return false;
}

} // namespace

namespace ECS {

void PawnAISystem::Update(Registry& registry, double /*delta_time*/, const GameContext* context) {
	if (!context || !context->map || !context->map->GetMap()) {
		return;
	}

	Map* map = context->map->GetMap();

	registry.ForEach<PawnStateComponent>([&](Entity entity, PawnStateComponent& state) {
		// Skip pawns that are in need-satisfaction states - they're handled by PawnNeedSatisfactionSystem
		if (state.status == kMoveToProvider || state.status == kWorking || state.status == kSleeping) {
			return;
		}
		
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
			MovementUtils::ClearMovement(registry, entity, context);
			return true;
		};

		auto finalize_result = MovementUtils::TryFinalizePath(registry, entity, context);

		if (movement.movement_finished) {
			state.has_wander_target = false;
			MovementUtils::ClearMovement(registry, entity, context);
		}

		if (!ensureTarget()) {
			return;
		}

		finalize_result = MovementUtils::TryFinalizePath(registry, entity, context);

		if (finalize_result == MovementUtils::PathFinalizationResult::kFailure) {
			state.has_wander_target = false;
			MovementUtils::ClearMovement(registry, entity, context);
			if (!ensureTarget()) {
				return;
			}
			MovementUtils::RequestPathTo(registry, entity, state.wander_target, context);
			return;
		}

		if (!movement.path_set) {
			MovementUtils::RequestPathTo(registry, entity, state.wander_target, context);
			return;
		}

		if (movement.movement_finished) {
			state.has_wander_target = false;
			MovementUtils::ClearMovement(registry, entity, context);
			if (ensureTarget()) {
				MovementUtils::RequestPathTo(registry, entity, state.wander_target, context);
			}
		}
	});
}

} // namespace ECS

