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
#include "../../include/ecs/system/PawnNeedsSystem.h"

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
	if (!instance.needs_system_) {
		instance.needs_system_ = new ECS::PawnNeedsSystem();
	}
	return instance;
}

// Ticks AI, pathing, movement, and needs systems against the shared registry.
void Systems::Update(double delta_time) {
	auto& registry = PawnECS::GetRegistry();
	needs_system_->Update(registry, delta_time);
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

