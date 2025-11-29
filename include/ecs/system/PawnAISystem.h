//------------------------------------------------------------------------------
// File: PawnAISystem.h
// Purpose: Declares the ECS system responsible for updating pawn behavior
//          state machines each frame.
//------------------------------------------------------------------------------
#ifndef ECS_PAWN_AI_SYSTEM_H
#define ECS_PAWN_AI_SYSTEM_H

#include "../System.h"

namespace ECS {

// Runs the pawn AI state machine entirely via ECS components.
class PawnAISystem : public IEcsSystem {
public:
// Evaluates AI logic for every entity owning a PawnStateComponent.
	void Update(Registry& registry, double delta_time, const GameContext* context = nullptr) override;
};

} // namespace ECS

#endif // ECS_PAWN_AI_SYSTEM_H

