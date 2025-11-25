//------------------------------------------------------------------------------
// File: PrisonerMovementSystem.h
// Purpose: Declares the ECS system that integrates TransformComponent position
//          using the direction produced by the path-follower.
//------------------------------------------------------------------------------
#ifndef ECS_PRISONER_MOVEMENT_SYSTEM_H
#define ECS_PRISONER_MOVEMENT_SYSTEM_H

#include "../System.h"

namespace ECS {

// Applies velocity to prisoner transforms based on movement components.
class PrisonerMovementSystem : public IEcsSystem {
public:
	// Moves every entity that has both Movement and Transform components.
	void Update(Registry& registry, double delta_time) override;
};

} // namespace ECS

#endif // ECS_PRISONER_MOVEMENT_SYSTEM_H

