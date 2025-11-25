//------------------------------------------------------------------------------
// File: PrisonerAISystem.h
// Purpose: Declares the ECS system responsible for updating prisoner behavior
//          state machines each frame.
//------------------------------------------------------------------------------
#ifndef ECS_PRISONER_AI_SYSTEM_H
#define ECS_PRISONER_AI_SYSTEM_H

#include "../System.h"

namespace ECS {

// Runs the prisoner AI state machine entirely via ECS components.
class PrisonerAISystem : public IEcsSystem {
public:
	// Evaluates AI logic for every entity owning a PrisonerStateComponent.
	void Update(Registry& registry, double delta_time) override;
};

} // namespace ECS

#endif // ECS_PRISONER_AI_SYSTEM_H

