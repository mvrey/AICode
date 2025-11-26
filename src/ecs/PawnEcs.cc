//------------------------------------------------------------------------------
// File: PawnEcs.cc
// Purpose: Implements the global registry accessor so legacy code can interact
//          with ECS entities without constructing registries per call-site.
//------------------------------------------------------------------------------
#include "../../include/ecs/PawnEcs.h"

namespace {
ECS::Registry g_pawn_registry;
}

namespace PawnECS {

// Returns the singleton pawn registry used across the game.
ECS::Registry& GetRegistry() {
	return g_pawn_registry;
}

// Creates a new ECS pawn entity within the shared registry.
ECS::Entity CreatePawnEntity() {
	return g_pawn_registry.CreateEntity();
}

// Destroys the entity and removes all of its registered components.
void DestroyPawnEntity(ECS::Entity entity) {
	g_pawn_registry.DestroyEntity(entity);
}

} // namespace PawnECS

