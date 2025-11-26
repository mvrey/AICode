//------------------------------------------------------------------------------
// File: PawnEcsManager.cc
// Purpose: Provides legacy-friendly wrappers for creating and destroying ECS
//          pawn entities.
//------------------------------------------------------------------------------
#include "../../include/ecs/PawnEcsManager.h"
#include "../../include/ecs/components/PawnComponents.h"

namespace PawnECS {

// Delegates to the global registry helper so callers avoid header dependencies.
ECS::Entity Manager::CreatePawnEntity() {
	return PawnECS::CreatePawnEntity();
}

// Validates the entity and removes it from the registry if it exists.
void Manager::DestroyPawnEntity(ECS::Entity entity) {
	if (entity.IsValid()) {
		PawnECS::DestroyPawnEntity(entity);
	}
}

} // namespace PawnECS

