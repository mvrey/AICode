#include "../../../include/ecs/system/PrisonerRenderSystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/TransformComponent.h"
#include "../../../include/ecs/components/SpriteComponent.h"

#include <MOMOS/draw.h>
#include <MOMOS/sprite.h>

namespace ECS {

void PrisonerRenderSystem::Update(Registry& registry, double delta_time) {
	registry.ForEach<SpriteComponent>([&](Entity entity, SpriteComponent& sprite) {
		if (!sprite.visible || sprite.sprite == nullptr)
			return;

		if (!registry.HasComponent<TransformComponent>(entity))
			return;

		const auto& transform = registry.GetComponent<TransformComponent>(entity);

		float draw_x = transform.position.x - sprite.width / 2.0f;
		float draw_y = transform.position.y - sprite.height / 2.0f;
		MOMOS::DrawSprite(sprite.sprite, draw_x, draw_y);
	});
}

} // namespace ECS

