//------------------------------------------------------------------------------
// File: PawnNeedSatisfactionSystem.cc
// Purpose: Implementation of PawnNeedSatisfactionSystem
//------------------------------------------------------------------------------
#include "../../../include/ecs/system/PawnNeedSatisfactionSystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/PawnStateComponent.h"
#include "../../../include/ecs/components/NeedsControllerComponent.h"
#include "../../../include/ecs/components/TransformComponent.h"
#include "../../../include/PawnAI/PawnAI.h"
#include "../../../include/FSM/PawnFSM.h"

namespace ECS {

void PawnNeedSatisfactionSystem::Update(Registry& registry, double delta_time) {
	PawnAI ai;
	PawnFSM fsm;

	// Update all pawns with needs controllers
	registry.ForEach<NeedsControllerComponent>([&](Entity entity, NeedsControllerComponent& needs_comp) {
		if (!needs_comp.controller) {
			return;
		}

		// Tick needs (apply decay)
		needs_comp.controller->Tick(delta_time);

		// Get pawn state and transform
		if (!registry.HasComponent<PawnStateComponent>(entity) ||
			!registry.HasComponent<TransformComponent>(entity)) {
			return;
		}

		auto& state = registry.GetComponent<PawnStateComponent>(entity);
		auto& transform = registry.GetComponent<TransformComponent>(entity);

		// If idle, let AI make a decision
		if (state.status == kIdle) {
			ai.Evaluate(registry, entity, transform.position, *needs_comp.controller);
		}

		// Update FSM for all states
		fsm.Update(registry, entity, delta_time);
	});
}

} // namespace ECS

