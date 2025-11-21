#ifndef ECS_TRANSFORM_COMPONENT_H
#define ECS_TRANSFORM_COMPONENT_H

#include <MOMOS/math.h>

namespace ECS {

struct TransformComponent {
	MOMOS::Vec2 position = { 0.0f, 0.0f };
	MOMOS::Vec2 direction = { 1.0f, 0.0f };
	float rotation = 0.0f;
};

} // namespace ECS

#endif // ECS_TRANSFORM_COMPONENT_H

