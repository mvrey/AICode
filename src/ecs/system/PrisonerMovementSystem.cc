#include "../../../include/ecs/system/PrisonerMovementSystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/MovementComponent.h"
#include "../../../include/ecs/components/TransformComponent.h"

#include <MOMOS/math.h>

namespace ECS {

void PrisonerMovementSystem::Update(Registry& registry, double delta_time) {
	registry.ForEach<MovementComponent>([&](Entity entity, MovementComponent& movement) {
		if (!registry.HasComponent<TransformComponent>(entity))
			return;

		auto& transform = registry.GetComponent<TransformComponent>(entity);

		transform.position.x += transform.direction.x * movement.speed * static_cast<float>(delta_time);
		transform.position.y += transform.direction.y * movement.speed * static_cast<float>(delta_time);
	});
}

} // namespace ECS

