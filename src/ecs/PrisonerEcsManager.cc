#include "../../include/ecs/PrisonerEcsManager.h"
#include "../../include/Agents/Prisoner.h"
#include "../../include/ecs/components/PrisonerComponents.h"

namespace PrisonerECS {

ECS::Entity Manager::CreatePrisonerEntity() {
	return PrisonerECS::CreatePrisonerEntity();
}

void Manager::DestroyPrisonerEntity(ECS::Entity entity) {
	if (entity.IsValid()) {
		PrisonerECS::DestroyPrisonerEntity(entity);
	}
}

} // namespace PrisonerECS

