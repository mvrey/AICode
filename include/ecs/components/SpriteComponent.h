//------------------------------------------------------------------------------
// File: SpriteComponent.h
// Purpose: Holds the rendering metadata needed to draw an entity, decoupling
//          sprite usage from legacy render paths.
//------------------------------------------------------------------------------
#ifndef ECS_SPRITE_COMPONENT_H
#define ECS_SPRITE_COMPONENT_H

#include <MOMOS/sprite.h>

namespace ECS {

// Stores the sprite handle, dimensions, and visibility so render systems can
// submit quads without querying Agent instances.
struct SpriteComponent {
	::MOMOS::SpriteHandle sprite = nullptr;
	float width = 0.0f;
	float height = 0.0f;
	bool visible = true;
};

} // namespace ECS

#endif // ECS_SPRITE_COMPONENT_H

