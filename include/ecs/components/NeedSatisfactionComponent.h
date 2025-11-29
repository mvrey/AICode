//------------------------------------------------------------------------------
// File: NeedSatisfactionComponent.h
// Purpose: Tracks a pawn's current need satisfaction task (provider, need, timer)
//------------------------------------------------------------------------------
#ifndef ECS_NEED_SATISFACTION_COMPONENT_H
#define ECS_NEED_SATISFACTION_COMPONENT_H

#include "../../Providers/INeedProvider.h"
#include "../../Needs/NeedId.h"

namespace ECS {

/// Tracks a pawn's current need satisfaction task
/// Used by the FSM to know which provider to use and which need to restore
struct NeedSatisfactionComponent {
	/// The provider the pawn is currently using or moving to
	INeedProvider* current_provider = nullptr;

	/// The need being satisfied
	NeedId target_need = NeedId::Hunger;

	/// Timer for the working state (how long the pawn has been working)
	double working_timer = 0.0;

	/// Whether the pawn has reached the provider location
	bool reached_provider = false;
};

} // namespace ECS

#endif // ECS_NEED_SATISFACTION_COMPONENT_H

