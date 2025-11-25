//------------------------------------------------------------------------------
// File: PrisonerEcs.h
// Purpose: Declares the public entry points for interacting with the prisoner
//          ECS registry from legacy systems.
//------------------------------------------------------------------------------
#ifndef PRISONER_ECS_H
#define PRISONER_ECS_H

#include "Registry.h"

namespace PrisonerECS {

// Grants global access to the prisoner-specific ECS registry.
ECS::Registry& GetRegistry();
// Creates a fully-registered prisoner entity with the required components.
ECS::Entity CreatePrisonerEntity();
// Cleans up all components and releases the entity back to the registry.
void DestroyPrisonerEntity(ECS::Entity entity);

} // namespace PrisonerECS

#endif // PRISONER_ECS_H

