//------------------------------------------------------------------------------
// File: PrisonerEcs.cc
// Purpose: Implements the global registry accessor so legacy code can interact
//          with ECS entities without constructing registries per call-site.
//------------------------------------------------------------------------------
#include "../../include/ecs/PrisonerEcs.h"

namespace {
ECS::Registry g_prisoner_registry;
}

namespace PrisonerECS {

// Returns the singleton prisoner registry used across the game.
ECS::Registry& GetRegistry() {
	return g_prisoner_registry;
}

// Creates a new ECS prisoner entity within the shared registry.
ECS::Entity CreatePrisonerEntity() {
	return g_prisoner_registry.CreateEntity();
}

// Destroys the entity and removes all of its registered components.
void DestroyPrisonerEntity(ECS::Entity entity) {
	g_prisoner_registry.DestroyEntity(entity);
}

} // namespace PrisonerECS

