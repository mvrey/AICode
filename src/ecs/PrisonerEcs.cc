#include "../../include/ecs/PrisonerEcs.h"

namespace {
ECS::Registry g_prisoner_registry;
}

namespace PrisonerECS {

ECS::Registry& GetRegistry() {
	return g_prisoner_registry;
}

ECS::Entity CreatePrisonerEntity() {
	return g_prisoner_registry.CreateEntity();
}

void DestroyPrisonerEntity(ECS::Entity entity) {
	g_prisoner_registry.DestroyEntity(entity);
}

} // namespace PrisonerECS

