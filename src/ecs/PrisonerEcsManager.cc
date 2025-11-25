//------------------------------------------------------------------------------
// File: PrisonerEcsManager.cc
// Purpose: Provides legacy-friendly wrappers for creating and destroying ECS
//          prisoner entities.
//------------------------------------------------------------------------------
#include "../../include/ecs/PrisonerEcsManager.h"
#include "../../include/Agents/Prisoner.h"
#include "../../include/ecs/components/PrisonerComponents.h"

namespace PrisonerECS {

// Delegates to the global registry helper so callers avoid header dependencies.
ECS::Entity Manager::CreatePrisonerEntity() {
	return PrisonerECS::CreatePrisonerEntity();
}

// Validates the entity and removes it from the registry if it exists.
void Manager::DestroyPrisonerEntity(ECS::Entity entity) {
	if (entity.IsValid()) {
		PrisonerECS::DestroyPrisonerEntity(entity);
	}
}

} // namespace PrisonerECS

