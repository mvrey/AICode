#include "../../../include/ecs/system/PrisonerAISystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/PrisonerComponents.h"
#include "../../../include/GameStatus.h"
#include "../../../include/PrisonMap.h"

#include <MOMOS/math.h>

namespace ECS {

void PrisonerAISystem::Update(Registry& registry, double delta_time) {
	registry.ForEach<PrisonerStateComponent>([&](Entity entity, PrisonerStateComponent& state) {
		auto& transform = registry.GetComponent<TransformComponent>(entity);
		auto& movement = registry.GetComponent<MovementComponent>(entity);

		// Basic behavior: if not escaping and alarm triggers, switch status.
		if (GameStatus::get()->alarm_mode_) {
			state.status = kEscaping;
		}

		// Simplified AI: just move horizontally based on working shift.
		transform.direction = state.status == kEscaping ? MOMOS::Vec2{ 1.0f, 0.0f } : MOMOS::Vec2{ -1.0f, 0.0f };
		movement.speed = (state.status == kEscaping ? 0.15f : 0.08f) * GameStatus::get()->simulation_speed_;
	});
}

} // namespace ECS

