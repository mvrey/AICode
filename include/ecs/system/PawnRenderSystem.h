//------------------------------------------------------------------------------
// File: PawnRenderSystem.h
// Purpose: Declares the rendering pass that draws ECS-managed pawns.
//------------------------------------------------------------------------------
#ifndef ECS_PAWN_RENDER_SYSTEM_H
#define ECS_PAWN_RENDER_SYSTEM_H

#include "../System.h"

namespace ECS {

// Draws sprites for every entity that has Sprite and Transform components.
class PawnRenderSystem : public IEcsSystem {
public:
	// Issues draw calls for the current frame.
	void Update(Registry& registry, double delta_time, const GameContext* context = nullptr) override;
};

} // namespace ECS

#endif // ECS_PAWN_RENDER_SYSTEM_H

