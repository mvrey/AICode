//------------------------------------------------------------------------------
// File: PawnNeedsSystem.cc
// Purpose: Updates needs decay (ticking NeedsController)
//          Note: NeedsComponent has been removed - UI reads directly from NeedsControllerComponent
//------------------------------------------------------------------------------
#include "../../../include/ecs/system/PawnNeedsSystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/NeedsControllerComponent.h"

namespace ECS {

void PawnNeedsSystem::Update(Registry& registry, double delta_time, const GameContext* /*context*/) {
	// Tick all NeedsControllers to apply decay
	// Note: The actual ticking is done in PawnNeedSatisfactionSystem, so this system
	// is now essentially empty. We keep it for potential future needs decay logic.
	// TODO: Consider removing this system if no longer needed, or merge with PawnNeedSatisfactionSystem
	(void)registry;
	(void)delta_time;
}

} // namespace ECS

