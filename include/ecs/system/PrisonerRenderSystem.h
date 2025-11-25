//------------------------------------------------------------------------------
// File: PrisonerRenderSystem.h
// Purpose: Declares the rendering pass that draws ECS-managed prisoners.
//------------------------------------------------------------------------------
#ifndef ECS_PRISONER_RENDER_SYSTEM_H
#define ECS_PRISONER_RENDER_SYSTEM_H

#include "../System.h"

namespace ECS {

// Draws sprites for every entity that has Sprite and Transform components.
class PrisonerRenderSystem : public IEcsSystem {
public:
	// Issues draw calls for the current frame.
	void Update(Registry& registry, double delta_time) override;
};

} // namespace ECS

#endif // ECS_PRISONER_RENDER_SYSTEM_H

