//------------------------------------------------------------------------------
// File: PawnNeedsSystem.cc
// Purpose: Implements the pawn needs system that decreases needs over time.
//------------------------------------------------------------------------------
#include "../../../include/ecs/system/PawnNeedsSystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/NeedsComponent.h"

#include <algorithm>

namespace ECS {

void PawnNeedsSystem::Update(Registry& registry, double delta_time) {
	const float delta_seconds = static_cast<float>(delta_time);
	
	registry.ForEach<NeedsComponent>([delta_seconds](Entity /*entity*/, NeedsComponent& needs) {
		// Decrease each need based on its rate
		needs.hunger -= needs.hunger_decrease_rate * delta_seconds;
		needs.energy -= needs.energy_decrease_rate * delta_seconds;
		needs.joy -= needs.joy_decrease_rate * delta_seconds;
		
		// Clamp values to [0, 1] range
		needs.hunger = std::max(0.0f, std::min(1.0f, needs.hunger));
		needs.energy = std::max(0.0f, std::min(1.0f, needs.energy));
		needs.joy = std::max(0.0f, std::min(1.0f, needs.joy));
	});
}

} // namespace ECS

