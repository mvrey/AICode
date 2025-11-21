#ifndef PRISONER_ECS_H
#define PRISONER_ECS_H

#include "Registry.h"

namespace PrisonerECS {

ECS::Registry& GetRegistry();
ECS::Entity CreatePrisonerEntity();
void DestroyPrisonerEntity(ECS::Entity entity);

} // namespace PrisonerECS

#endif // PRISONER_ECS_H

