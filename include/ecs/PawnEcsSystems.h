//------------------------------------------------------------------------------
// File: PawnEcsSystems.h
// Purpose: Declares the façade that wires all pawn ECS systems together so
//          the legacy game loop can tick/update/render them with minimal code.
//------------------------------------------------------------------------------
#ifndef PAWN_ECS_SYSTEMS_H
#define PAWN_ECS_SYSTEMS_H

#include "Registry.h"

namespace ECS {
class PawnAISystem;
class PawnPathFollowSystem;
class PawnMovementSystem;
class PawnRenderSystem;
class PawnNeedsSystem;
class PawnNeedSatisfactionSystem;
} // namespace ECS

namespace PawnECS {

// Aggregates all pawn ECS systems and exposes a simple update/render API.
class Systems {
public:
	// Returns the singleton instance, creating sub-systems on first use.
	static Systems& Get();

	// Runs AI, path following, and movement passes for the frame.
	void Update(double delta_time);
	// Runs the render system pass for the frame.
	void Render(double delta_time);

private:
	Systems() = default;

	ECS::PawnAISystem* ai_system_ = nullptr;
	ECS::PawnPathFollowSystem* path_system_ = nullptr;
	ECS::PawnMovementSystem* movement_system_ = nullptr;
	ECS::PawnRenderSystem* render_system_ = nullptr;
	ECS::PawnNeedsSystem* needs_system_ = nullptr;
	ECS::PawnNeedSatisfactionSystem* need_satisfaction_system_ = nullptr;
};

} // namespace PawnECS

#endif // PAWN_ECS_SYSTEMS_H

