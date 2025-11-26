//------------------------------------------------------------------------------
// File: PrisonerRenderSystem.cc
// Purpose: Draws sprites for ECS-managed prisoners based on their transforms.
//------------------------------------------------------------------------------
#include "../../../include/Camera.h"
#include "../../../include/ecs/system/PrisonerRenderSystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/TransformComponent.h"
#include "../../../include/ecs/components/SpriteComponent.h"

#include <MOMOS/draw.h>
#include <MOMOS/sprite.h>

namespace ECS {

// Iterates every SpriteComponent and issues MOMOS draw calls if visible.
void PrisonerRenderSystem::Update(Registry& registry, double delta_time) {
	registry.ForEach<SpriteComponent>([&](Entity entity, SpriteComponent& sprite) {
		if (!sprite.visible || sprite.sprite == nullptr)
			return;

		if (!registry.HasComponent<TransformComponent>(entity))
			return;

		const auto& transform = registry.GetComponent<TransformComponent>(entity);

		::MOMOS::Vec2 screen_position = Camera::WorldToScreen(transform.position);
		float zoom = Camera::Zoom();
		float half_width = sprite.width * 0.5f * zoom;
		float half_height = sprite.height * 0.5f * zoom;

		::MOMOS::SpriteTransform sprite_transform{};
		sprite_transform.x = screen_position.x - half_width;
		sprite_transform.y = screen_position.y - half_height;
		sprite_transform.scale_x = zoom;
		sprite_transform.scale_y = zoom;
		MOMOS::DrawSprite(sprite.sprite, sprite_transform);
	});
}

} // namespace ECS

