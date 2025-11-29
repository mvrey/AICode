//------------------------------------------------------------------------------
// File: PawnNeedSatisfactionSystem.cc
// Purpose: Implementation of PawnNeedSatisfactionSystem
//------------------------------------------------------------------------------
#include "../../../include/ecs/system/PawnNeedSatisfactionSystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/PawnStateComponent.h"
#include "../../../include/ecs/components/NeedsControllerComponent.h"
#include "../../../include/ecs/components/TransformComponent.h"
#include "../../../include/ecs/components/MovementComponent.h"
#include "../../../include/ecs/PawnMovementUtils.h"
#include "../../../include/Needs/INeed.h"
#include "../../../include/PawnAI/PawnAI.h"
#include "../../../include/FSM/PawnFSM.h"
#include "../../../include/config.h"

namespace ECS {

void PawnNeedSatisfactionSystem::Update(Registry& registry, double delta_time, const GameContext* context) {
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

		// Check if pawn has urgent needs
		INeed* urgent_need = needs_comp.controller->GetMostUrgentNeed();
		bool has_urgent_need = urgent_need && urgent_need->IsUrgent();

		// If pawn has urgent needs and is not already handling them, interrupt current behavior
		if (has_urgent_need && 
			state.status != kMoveToProvider && 
			state.status != kWorking && 
			state.status != kSleeping) {
			// Force pawn to idle so AI can make a decision
			state.status = kIdle;
			// Clear any wandering targets
			state.has_wander_target = false;
			// Clear movement if pawn is moving
			if (registry.HasComponent<MovementComponent>(entity)) {
				auto& movement = registry.GetComponent<MovementComponent>(entity);
				PawnECS::MovementUtils::ClearMovement(registry, entity, context);
			}
		}

		// If idle, let AI make a decision
		if (state.status == kIdle) {
			ai.Evaluate(registry, entity, transform.position, *needs_comp.controller);
		}

		// Update FSM for all states
		fsm.Update(registry, entity, delta_time, context);
	});
}

} // namespace ECS

