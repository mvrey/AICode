//------------------------------------------------------------------------------
// File: PrisonerEcsManager.h
// Purpose: Declares helper routines that bridge legacy Prisoner construction
//          with the ECS entity lifecycle.
//------------------------------------------------------------------------------
#ifndef PRISONER_ECS_MANAGER_H
#define PRISONER_ECS_MANAGER_H

#include "PrisonerEcs.h"

class Prisoner;

namespace PrisonerECS {

// Thin static manager that exposes lifecycle helpers to legacy callers.
struct Manager {
	// Spawns a new ECS prisoner entity with all mandatory components.
	static ECS::Entity CreatePrisonerEntity();
	// Cleans up and destroys the entity plus attached components.
	static void DestroyPrisonerEntity(ECS::Entity entity);
};

} // namespace PrisonerECS

#endif // PRISONER_ECS_MANAGER_H

