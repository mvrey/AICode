//------------------------------------------------------------------------------
// File: PawnNeedsSystem.cc
// Purpose: Syncs NeedsController with NeedsComponent for UI compatibility
//          The actual need decay is handled by NeedsController, this just syncs values
//------------------------------------------------------------------------------
#include "../../../include/ecs/system/PawnNeedsSystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/NeedsComponent.h"
#include "../../../include/ecs/components/NeedsControllerComponent.h"
#include "../../../include/Needs/NeedId.h"
#include "../../../include/Needs/INeed.h"

namespace ECS {

void PawnNeedsSystem::Update(Registry& registry, double /*delta_time*/) {
	// Sync NeedsController values to NeedsComponent for UI compatibility
	registry.ForEach<NeedsControllerComponent>([&registry](Entity entity, NeedsControllerComponent& needs_comp) {
		if (!needs_comp.controller) {
			return;
		}

		// Get or create NeedsComponent for UI compatibility
		if (!registry.HasComponent<NeedsComponent>(entity)) {
			registry.AddComponent<NeedsComponent>(entity);
		}

		auto& needs = registry.GetComponent<NeedsComponent>(entity);

		// Sync values from NeedsController
		INeed* hunger_need = needs_comp.controller->GetNeed(NeedId::Hunger);
		if (hunger_need) {
			needs.hunger = hunger_need->GetValue();
			needs.hunger_decrease_rate = hunger_need->GetDecayRate();
		}

		INeed* energy_need = needs_comp.controller->GetNeed(NeedId::Energy);
		if (energy_need) {
			needs.energy = energy_need->GetValue();
			needs.energy_decrease_rate = energy_need->GetDecayRate();
		}

		INeed* joy_need = needs_comp.controller->GetNeed(NeedId::Joy);
		if (joy_need) {
			needs.joy = joy_need->GetValue();
			needs.joy_decrease_rate = joy_need->GetDecayRate();
		}
	});
}

} // namespace ECS

