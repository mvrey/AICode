//------------------------------------------------------------------------------
// File: PawnEcs.h
// Purpose: Declares the public entry points for interacting with the pawn
//          ECS registry from legacy systems.
//------------------------------------------------------------------------------
#ifndef PAWN_ECS_H
#define PAWN_ECS_H

#include "Registry.h"

namespace PawnECS {

// Grants global access to the pawn-specific ECS registry.
ECS::Registry& GetRegistry();
// Creates a fully-registered pawn entity with the required components.
ECS::Entity CreatePawnEntity();
// Cleans up all components and releases the entity back to the registry.
void DestroyPawnEntity(ECS::Entity entity);

} // namespace PawnECS

#endif // PAWN_ECS_H

