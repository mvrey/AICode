//------------------------------------------------------------------------------
// File: PawnMovementSystem.h
// Purpose: Declares the ECS system that integrates TransformComponent position
//          using the direction produced by the path-follower.
//------------------------------------------------------------------------------
#ifndef ECS_PAWN_MOVEMENT_SYSTEM_H
#define ECS_PAWN_MOVEMENT_SYSTEM_H

#include "../System.h"

namespace ECS {

// Applies velocity to pawn transforms based on movement components.
class PawnMovementSystem : public IEcsSystem {
public:
	// Moves every entity that has both Movement and Transform components.
	void Update(Registry& registry, double delta_time, const GameContext* context = nullptr) override;
};

} // namespace ECS

#endif // ECS_PAWN_MOVEMENT_SYSTEM_H

