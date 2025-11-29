//------------------------------------------------------------------------------
// File: PawnAI.h
// Purpose: Decision layer that queries needs and providers, selects actions
//------------------------------------------------------------------------------
#ifndef PAWN_AI_H
#define PAWN_AI_H

#include "../Needs/NeedsController.h"
#include "../Providers/ProviderRegistry.h"
#include "../ecs/Entity.h"
#include <MOMOS/math.h>

namespace ECS {
	class Registry;
}

/// Decision layer for pawn AI
/// Queries needs and providers, selects the most urgent need and nearest provider
/// Does NOT contain movement, animation, or timing logic - only decision making
class PawnAI {
public:
	/// Evaluate the pawn's needs and decide on an action
	/// @param registry The ECS registry
	/// @param entity The pawn entity
	/// @param position The pawn's current position
	/// @param needs_controller The pawn's needs controller
	/// @return true if a decision was made and the FSM should act, false if idle
	bool Evaluate(ECS::Registry& registry, ECS::Entity entity, 
		const ::MOMOS::Vec2& position, NeedsController& needs_controller);

private:
	/// Find a provider for the most urgent need
	INeedProvider* FindProviderForNeed(NeedId need_id, const ::MOMOS::Vec2& position) const;
};

#endif // PAWN_AI_H

