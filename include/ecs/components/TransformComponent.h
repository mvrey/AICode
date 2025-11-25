//------------------------------------------------------------------------------
// File: TransformComponent.h
// Purpose: Encapsulates basic spatial data (position, facing, rotation) so ECS
//          systems can move and render entities without referencing AgentBody.
//------------------------------------------------------------------------------
#ifndef ECS_TRANSFORM_COMPONENT_H
#define ECS_TRANSFORM_COMPONENT_H

#include <MOMOS/math.h>

namespace ECS {

// Tracks world-space position, normalized direction, and optional rotation for
// a single entity.
struct TransformComponent {
	::MOMOS::Vec2 position = { 0.0f, 0.0f };
	::MOMOS::Vec2 direction = { 1.0f, 0.0f };
	float rotation = 0.0f;
};

} // namespace ECS

#endif // ECS_TRANSFORM_COMPONENT_H

