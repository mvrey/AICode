#ifndef ECS_PAWN_FACTORY_H
#define ECS_PAWN_FACTORY_H

#include "PawnEcs.h"

namespace PawnECS {

/// Spawns a new pawn entity with default movement, rendering, and state data.
ECS::Entity SpawnPawn(short working_shift = 0);

} // namespace PawnECS

#endif // ECS_PAWN_FACTORY_H

