#ifndef PRISONER_ECS_MANAGER_H
#define PRISONER_ECS_MANAGER_H

#include "PrisonerEcs.h"

class Prisoner;

namespace PrisonerECS {

struct Manager {
	static ECS::Entity CreatePrisonerEntity(Nothing);
	static void DestroyPrisonerEntity(ECS::Entity entity);
};

} // namespace PrisonerECS

#endif // PRISONER_ECS_MANAGER_H

