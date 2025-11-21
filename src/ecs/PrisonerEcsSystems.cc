#include "../../include/ecs/PrisonerEcsSystems.h"
#include "../../include/ecs/PrisonerEcs.h"
#include "../../include/ecs/system/PrisonerAISystem.h"
#include "../../include/ecs/system/PrisonerMovementSystem.h"
#include "../../include/ecs/system/PrisonerRenderSystem.h"

namespace PrisonerECS {

Systems& Systems::Get() {
	static Systems instance;
	if (!instance.ai_system_) {
		instance.ai_system_ = new ECS::PrisonerAISystem();
	}
	if (!instance.movement_system_) {
		instance.movement_system_ = new ECS::PrisonerMovementSystem();
	}
	if (!instance.render_system_) {
		instance.render_system_ = new ECS::PrisonerRenderSystem();
	}
	return instance;
}

void Systems::Update(double delta_time) {
	auto& registry = PrisonerECS::GetRegistry();
	ai_system_->Update(registry, delta_time);
	movement_system_->Update(registry, delta_time);
}

void Systems::Render(double delta_time) {
	auto& registry = PrisonerECS::GetRegistry();
	render_system_->Update(registry, delta_time);
}

} // namespace PrisonerECS

