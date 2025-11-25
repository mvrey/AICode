//------------------------------------------------------------------------------
// File: PrisonerEcsSystems.cc
// Purpose: Implements the façade that wires every prisoner ECS system together
//          and exposes simple update/render entry points.
//------------------------------------------------------------------------------
#include "../../include/ecs/PrisonerEcsSystems.h"
#include "../../include/ecs/PrisonerEcs.h"
#include "../../include/ecs/system/PrisonerAISystem.h"
#include "../../include/ecs/system/PrisonerPathFollowSystem.h"
#include "../../include/ecs/system/PrisonerMovementSystem.h"
#include "../../include/ecs/system/PrisonerRenderSystem.h"

namespace PrisonerECS {

// Lazily instantiates (and returns) the shared system collection.
Systems& Systems::Get() {
	static Systems instance;
	if (!instance.ai_system_) {
		instance.ai_system_ = new ECS::PrisonerAISystem();
	}
	if (!instance.path_system_) {
		instance.path_system_ = new ECS::PrisonerPathFollowSystem();
	}
	if (!instance.movement_system_) {
		instance.movement_system_ = new ECS::PrisonerMovementSystem();
	}
	if (!instance.render_system_) {
		instance.render_system_ = new ECS::PrisonerRenderSystem();
	}
	return instance;
}

// Ticks AI, pathing, and movement systems against the shared registry.
void Systems::Update(double delta_time) {
	auto& registry = PrisonerECS::GetRegistry();
	ai_system_->Update(registry, delta_time);
	path_system_->Update(registry, delta_time);
	movement_system_->Update(registry, delta_time);
}

// Executes the render pass for ECS prisoners.
void Systems::Render(double delta_time) {
	auto& registry = PrisonerECS::GetRegistry();
	render_system_->Update(registry, delta_time);
}

} // namespace PrisonerECS

