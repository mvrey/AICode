//------------------------------------------------------------------------------
// File: PawnPathFollowSystem.h
// Purpose: Declares the ECS system that advances entities along their queued
//          deterministic path steps.
//------------------------------------------------------------------------------
#ifndef ECS_PAWN_PATH_FOLLOW_SYSTEM_H
#define ECS_PAWN_PATH_FOLLOW_SYSTEM_H

#include "../System.h"

namespace ECS {

// Validates path cells and updates facing/step indices for active routes.
class PawnPathFollowSystem : public IEcsSystem {
public:
	// Processes all MovementComponents that currently have active paths.
	void Update(Registry& registry, double delta_time) override;
};

} // namespace ECS

#endif // ECS_PAWN_PATH_FOLLOW_SYSTEM_H

