//------------------------------------------------------------------------------
// File: PawnFSM.h
// Purpose: FSM system that handles pawn state transitions and state logic
//          Does NOT contain decision-making logic - only state execution
//------------------------------------------------------------------------------
#ifndef PAWN_FSM_H
#define PAWN_FSM_H

#include "../ecs/Entity.h"
#include "../config.h"

namespace ECS {
	class Registry;
}

/// FSM system that executes pawn states
/// Handles movement, working timers, and need restoration
/// Does NOT make decisions - those come from PawnAI
class PawnFSM {
public:
	/// Update the FSM for a pawn entity
	/// @param registry The ECS registry
	/// @param entity The pawn entity
	/// @param delta_time Time since last update
	void Update(ECS::Registry& registry, ECS::Entity entity, double delta_time);

private:
	/// Handle Idle state
	void HandleIdle(ECS::Registry& registry, ECS::Entity entity);

	/// Handle MoveToProvider state - moves pawn to provider location
	void HandleMoveToProvider(ECS::Registry& registry, ECS::Entity entity, double delta_time);

	/// Handle Working state - timer-based interaction with provider
	void HandleWorking(ECS::Registry& registry, ECS::Entity entity, double delta_time);

	/// Handle Sleeping state - passive energy restoration
	void HandleSleeping(ECS::Registry& registry, ECS::Entity entity, double delta_time);

	/// Check if pawn has reached the provider location
	bool HasReachedProvider(ECS::Registry& registry, ECS::Entity entity) const;
};

#endif // PAWN_FSM_H

