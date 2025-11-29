//------------------------------------------------------------------------------
// File: PawnFSM.cc
// Purpose: Implementation of PawnFSM
//------------------------------------------------------------------------------
#include "../../include/FSM/PawnFSM.h"
#include "../../include/Core/GameContext.h"
#include "../../include/ecs/components/PawnStateComponent.h"
#include "../../include/ecs/components/NeedSatisfactionComponent.h"
#include "../../include/ecs/components/NeedsControllerComponent.h"
#include "../../include/ecs/components/TransformComponent.h"
#include "../../include/ecs/components/MovementComponent.h"
#include "../../include/ecs/PawnMovementUtils.h"
#include "../../include/Needs/NeedsController.h"
#include "../../include/Needs/INeed.h"
#include "../../include/Needs/NeedId.h"
#include "../../include/Providers/INeedProvider.h"
#include "../../include/Providers/ProviderRegistry.h"
#include <cmath>

namespace MovementUtils = PawnECS::MovementUtils;

void PawnFSM::Update(ECS::Registry& registry, ECS::Entity entity, double delta_time, const GameContext* context) {
	if (!registry.HasComponent<ECS::PawnStateComponent>(entity)) {
		return;
	}

	auto& state = registry.GetComponent<ECS::PawnStateComponent>(entity);

	switch (state.status) {
		case kIdle:
			HandleIdle(registry, entity);
			break;
		case kMoveToProvider:
			HandleMoveToProvider(registry, entity, delta_time, context);
			break;
		case kWorking:
			HandleWorking(registry, entity, delta_time);
			break;
		case kSleeping:
			HandleSleeping(registry, entity, delta_time);
			break;
		default:
			// Handle other states (existing legacy states)
			break;
	}
}

void PawnFSM::HandleIdle(ECS::Registry& registry, ECS::Entity entity) {
	// Idle state - pawn is waiting for AI to make a decision
	// No action needed here, PawnAI will transition to other states
}

void PawnFSM::HandleMoveToProvider(ECS::Registry& registry, ECS::Entity entity, double delta_time, const GameContext* context) {
	if (!registry.HasComponent<ECS::NeedSatisfactionComponent>(entity)) {
		return;
	}

	auto& satisfaction = registry.GetComponent<ECS::NeedSatisfactionComponent>(entity);
	if (!satisfaction.current_provider) {
		// No provider, go back to idle
		if (registry.HasComponent<ECS::PawnStateComponent>(entity)) {
			auto& state = registry.GetComponent<ECS::PawnStateComponent>(entity);
			state.status = kIdle;
		}
		return;
	}

	// Check if we've reached the provider
	if (HasReachedProvider(registry, entity)) {
		satisfaction.reached_provider = true;
		
		// Transition to Working state
		if (registry.HasComponent<ECS::PawnStateComponent>(entity)) {
			auto& state = registry.GetComponent<ECS::PawnStateComponent>(entity);
			state.status = kWorking;
			satisfaction.working_timer = 0.0;
		}
		return;
	}

	// Request path to provider if not already moving
	if (registry.HasComponent<ECS::MovementComponent>(entity)) {
		auto& movement = registry.GetComponent<ECS::MovementComponent>(entity);
		if (!movement.path_set && !movement.movement_finished) {
			::MOMOS::Vec2 provider_pos = satisfaction.current_provider->GetPosition();
			MovementUtils::RequestPathTo(registry, entity, provider_pos, context);
		}
	}
}

void PawnFSM::HandleWorking(ECS::Registry& registry, ECS::Entity entity, double delta_time) {
	if (!registry.HasComponent<ECS::NeedSatisfactionComponent>(entity)) {
		return;
	}

	auto& satisfaction = registry.GetComponent<ECS::NeedSatisfactionComponent>(entity);
	if (!satisfaction.current_provider) {
		// Provider disappeared, go back to idle
		if (registry.HasComponent<ECS::PawnStateComponent>(entity)) {
			auto& state = registry.GetComponent<ECS::PawnStateComponent>(entity);
			state.status = kIdle;
		}
		return;
	}

	// Check if provider is still available
	if (!satisfaction.current_provider->IsAvailable()) {
		// Provider no longer available, go back to idle
		if (registry.HasComponent<ECS::PawnStateComponent>(entity)) {
			auto& state = registry.GetComponent<ECS::PawnStateComponent>(entity);
			state.status = kIdle;
		}
		return;
	}

	// Update working timer
	satisfaction.working_timer += delta_time;
	double use_duration = satisfaction.current_provider->GetUseDuration();

	if (satisfaction.working_timer >= use_duration) {
		// Working is complete - restore need and use provider
		
		// Restore the need using the needs controller
		if (registry.HasComponent<ECS::NeedsControllerComponent>(entity)) {
			auto& needs_comp = registry.GetComponent<ECS::NeedsControllerComponent>(entity);
			if (needs_comp.controller) {
				INeed* need = needs_comp.controller->GetNeed(satisfaction.target_need);
				if (need) {
					float restore_amount = satisfaction.current_provider->GetRestoreAmount();
					need->Restore(restore_amount);
				}
			}
		}

		// Call provider's OnUsed (this may destroy the provider)
		satisfaction.current_provider->OnUsed();

		// Unregister provider if it was destroyed
		ProviderRegistry::Get().UnregisterProvider(satisfaction.current_provider);

		// Clear satisfaction component
		registry.RemoveComponent<ECS::NeedSatisfactionComponent>(entity);

		// Return to idle
		if (registry.HasComponent<ECS::PawnStateComponent>(entity)) {
			auto& state = registry.GetComponent<ECS::PawnStateComponent>(entity);
			state.status = kIdle;
		}
	}
}

void PawnFSM::HandleSleeping(ECS::Registry& registry, ECS::Entity entity, double delta_time) {
	// Passive state that restores energy over time
	if (registry.HasComponent<ECS::NeedsControllerComponent>(entity)) {
		auto& needs_comp = registry.GetComponent<ECS::NeedsControllerComponent>(entity);
		if (needs_comp.controller) {
			INeed* energy_need = needs_comp.controller->GetNeed(NeedId::Energy);
			if (energy_need) {
				// Restore energy slowly while sleeping (0.1 per second)
				// delta_time is in milliseconds, convert to seconds
				const float restore_rate = 0.1f;
				float delta_seconds = static_cast<float>(delta_time) / 1000.0f;
				energy_need->Restore(restore_rate * delta_seconds);
			}
		}
	}
}

bool PawnFSM::HasReachedProvider(ECS::Registry& registry, ECS::Entity entity) const {
	if (!registry.HasComponent<ECS::TransformComponent>(entity) ||
		!registry.HasComponent<ECS::NeedSatisfactionComponent>(entity)) {
		return false;
	}

	const auto& transform = registry.GetComponent<ECS::TransformComponent>(entity);
	const auto& satisfaction = registry.GetComponent<ECS::NeedSatisfactionComponent>(entity);

	if (!satisfaction.current_provider) {
		return false;
	}

	::MOMOS::Vec2 provider_pos = satisfaction.current_provider->GetPosition();
	
	// Check if we're close enough (within 32 pixels)
	const float reach_distance = 32.0f;
	float dx = transform.position.x - provider_pos.x;
	float dy = transform.position.y - provider_pos.y;
	float distance = std::sqrt(dx * dx + dy * dy);

	return distance <= reach_distance;
}

