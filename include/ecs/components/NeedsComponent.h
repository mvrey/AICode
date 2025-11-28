//------------------------------------------------------------------------------
// File: NeedsComponent.h
// Purpose: Declares the component that tracks pawn needs (Hunger, Energy, Joy)
//          and their decrease rates.
//------------------------------------------------------------------------------
#ifndef ECS_NEEDS_COMPONENT_H
#define ECS_NEEDS_COMPONENT_H

namespace ECS {

struct NeedsComponent {
	// Current need values in range [0, 1]
	// 1.0 = fully satisfied, 0.0 = completely depleted
	float hunger = 1.0f;
	float energy = 1.0f;
	float joy = 1.0f;
	
	// Decrease rates per second for each need
	// These are assigned at pawn creation and remain constant
	float hunger_decrease_rate = 0.0f;  // per second
	float energy_decrease_rate = 0.0f; // per second
	float joy_decrease_rate = 0.0f;     // per second
};

} // namespace ECS

#endif // ECS_NEEDS_COMPONENT_H

