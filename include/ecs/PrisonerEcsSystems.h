//------------------------------------------------------------------------------
// File: PrisonerEcsSystems.h
// Purpose: Declares the façade that wires all prisoner ECS systems together so
//          the legacy game loop can tick/update/render them with minimal code.
//------------------------------------------------------------------------------
#ifndef PRISONER_ECS_SYSTEMS_H
#define PRISONER_ECS_SYSTEMS_H

#include "Registry.h"

namespace ECS {
class PrisonerAISystem;
class PrisonerPathFollowSystem;
class PrisonerMovementSystem;
class PrisonerRenderSystem;
} // namespace ECS

namespace PrisonerECS {

// Aggregates all prisoner ECS systems and exposes a simple update/render API.
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

	ECS::PrisonerAISystem* ai_system_ = nullptr;
	ECS::PrisonerPathFollowSystem* path_system_ = nullptr;
	ECS::PrisonerMovementSystem* movement_system_ = nullptr;
	ECS::PrisonerRenderSystem* render_system_ = nullptr;
};

} // namespace PrisonerECS

#endif // PRISONER_ECS_SYSTEMS_H

