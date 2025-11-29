//------------------------------------------------------------------------------
// File: PawnAI.cc
// Purpose: Implementation of PawnAI decision layer
//------------------------------------------------------------------------------
#include "../../include/PawnAI/PawnAI.h"
#include "../../include/ecs/Registry.h"
#include "../../include/ecs/components/NeedSatisfactionComponent.h"
#include "../../include/ecs/components/PawnStateComponent.h"
#include "../../include/config.h"

bool PawnAI::Evaluate(ECS::Registry& registry, ECS::Entity entity,
	const ::MOMOS::Vec2& position, NeedsController& needs_controller)
{
	// Query the needs controller for the most urgent need
	INeed* urgent_need = needs_controller.GetMostUrgentNeed();
	if (!urgent_need) {
		// No urgent needs, pawn can be idle
		return false;
	}

	// Check if the need is actually urgent (below threshold)
	if (!urgent_need->IsUrgent()) {
		// Need is not urgent yet, pawn can be idle
		return false;
	}

	// Find a provider for this need
	INeedProvider* provider = FindProviderForNeed(urgent_need->GetId(), position);
	if (!provider) {
		// No provider available for this need
		return false;
	}

	// Set up the need satisfaction component
	if (!registry.HasComponent<ECS::NeedSatisfactionComponent>(entity)) {
		registry.AddComponent<ECS::NeedSatisfactionComponent>(entity);
	}

	auto& satisfaction = registry.GetComponent<ECS::NeedSatisfactionComponent>(entity);
	satisfaction.current_provider = provider;
	satisfaction.target_need = urgent_need->GetId();
	satisfaction.working_timer = 0.0;
	satisfaction.reached_provider = false;

	// Update the pawn state to indicate we're moving to a provider
	if (registry.HasComponent<ECS::PawnStateComponent>(entity)) {
		auto& state = registry.GetComponent<ECS::PawnStateComponent>(entity);
		state.status = kMoveToProvider;
	}

	return true;
}

INeedProvider* PawnAI::FindProviderForNeed(NeedId need_id, const ::MOMOS::Vec2& position) const {
	ProviderRegistry& registry = ProviderRegistry::Get();
	return registry.FindNearestProvider(need_id, position);
}

