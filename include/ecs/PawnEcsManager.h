//------------------------------------------------------------------------------
// File: PawnEcsManager.h
// Purpose: Declares helper routines that bridge legacy Pawn construction
//          with the ECS entity lifecycle.
//------------------------------------------------------------------------------
#ifndef PAWN_ECS_MANAGER_H
#define PAWN_ECS_MANAGER_H

#include "PawnEcs.h"

class Pawn;

namespace PawnECS {

// Thin static manager that exposes lifecycle helpers to legacy callers.
struct Manager {
	// Spawns a new ECS pawn entity with all mandatory components.
	static ECS::Entity CreatePawnEntity();
	// Cleans up and destroys the entity plus attached components.
	static void DestroyPawnEntity(ECS::Entity entity);
};

} // namespace PawnECS

#endif // PAWN_ECS_MANAGER_H

