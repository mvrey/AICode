//------------------------------------------------------------------------------
// File: PawnNeedsSystem.h
// Purpose: Declares the ECS system responsible for updating pawn needs each frame.
//------------------------------------------------------------------------------
#ifndef ECS_PAWN_NEEDS_SYSTEM_H
#define ECS_PAWN_NEEDS_SYSTEM_H

#include "../System.h"

namespace ECS {

// Updates needs (Hunger, Energy, Joy) for every entity owning a NeedsComponent.
class PawnNeedsSystem : public IEcsSystem {
public:
	// Decreases needs based on their decrease rates and delta_time.
	void Update(Registry& registry, double delta_time, const GameContext* context = nullptr) override;
};

} // namespace ECS

#endif // ECS_PAWN_NEEDS_SYSTEM_H

