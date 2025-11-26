//------------------------------------------------------------------------------
// File: PawnEcsSystems.cc
// Purpose: Implements the façade that wires every pawn ECS system together
//          and exposes simple update/render entry points.
//------------------------------------------------------------------------------
#include "../../include/ecs/PawnEcsSystems.h"
#include "../../include/ecs/PawnEcs.h"
#include "../../include/ecs/system/PawnAISystem.h"
#include "../../include/ecs/system/PawnPathFollowSystem.h"
#include "../../include/ecs/system/PawnMovementSystem.h"
#include "../../include/ecs/system/PawnRenderSystem.h"

namespace PawnECS {

// Lazily instantiates (and returns) the shared system collection.
Systems& Systems::Get() {
	static Systems instance;
	if (!instance.ai_system_) {
		instance.ai_system_ = new ECS::PawnAISystem();
	}
	if (!instance.path_system_) {
		instance.path_system_ = new ECS::PawnPathFollowSystem();
	}
	if (!instance.movement_system_) {
		instance.movement_system_ = new ECS::PawnMovementSystem();
	}
	if (!instance.render_system_) {
		instance.render_system_ = new ECS::PawnRenderSystem();
	}
	return instance;
}

// Ticks AI, pathing, and movement systems against the shared registry.
void Systems::Update(double delta_time) {
	auto& registry = PawnECS::GetRegistry();
	ai_system_->Update(registry, delta_time);
	path_system_->Update(registry, delta_time);
	movement_system_->Update(registry, delta_time);
}

// Executes the render pass for ECS pawns.
void Systems::Render(double delta_time) {
	auto& registry = PawnECS::GetRegistry();
	render_system_->Update(registry, delta_time);
}

} // namespace PawnECS

