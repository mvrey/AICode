#ifndef ECS_PRISONER_FACTORY_H
#define ECS_PRISONER_FACTORY_H

#include "PrisonerEcs.h"

namespace PrisonerECS {

/// Spawns a new prisoner entity with default movement, rendering, and state data.
ECS::Entity SpawnPrisoner(short working_shift = 0);

} // namespace PrisonerECS

#endif // ECS_PRISONER_FACTORY_H

