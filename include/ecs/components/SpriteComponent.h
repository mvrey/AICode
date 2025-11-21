#ifndef ECS_SPRITE_COMPONENT_H
#define ECS_SPRITE_COMPONENT_H

#include <MOMOS/sprite.h>

namespace ECS {

struct SpriteComponent {
	::MOMOS::SpriteHandle sprite = nullptr;
	float width = 0.0f;
	float height = 0.0f;
	bool visible = true;
};

} // namespace ECS

#endif // ECS_SPRITE_COMPONENT_H

