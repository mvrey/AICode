//------------------------------------------------------------------------------
// File: PrisonerMovementUtils.h
// Purpose: Declares helper routines for manipulating ECS movement state without
//          touching legacy Prisoner objects.
//------------------------------------------------------------------------------
#ifndef PRISONER_ECS_MOVEMENT_UTILS_H
#define PRISONER_ECS_MOVEMENT_UTILS_H

#include <vector>
#include "./Registry.h"
#include "../PrisonMap.h"
#include "../Pathfinding/cost_map.h"
#include <MOMOS/math.h>
#include "components/MovementComponent.h"
#include "components/TransformComponent.h"

namespace PrisonerECS {
namespace MovementUtils {

void ClearMovement(ECS::Registry& registry, ECS::Entity entity);
bool TryFinalizePath(ECS::Registry& registry, ECS::Entity entity);
bool RequestPathTo(ECS::Registry& registry, ECS::Entity entity, const ::MOMOS::Vec2& destination);
bool BuildRoomWaypointPath(ECS::Registry& registry, ECS::Entity entity, const Room& room, PrisonMap* prison, CostMap* map);
void SetDoorRouteActive(ECS::Registry& registry, ECS::Entity entity, bool active);
void SetEscapeRouteActive(ECS::Registry& registry, ECS::Entity entity, bool active);
void CycleDoorTarget(ECS::Registry& registry, ECS::Entity entity, int totalDoors);
void SetDoorTarget(ECS::Registry& registry, ECS::Entity entity, int target);

} // namespace MovementUtils
} // namespace PrisonerECS

#endif // PRISONER_ECS_MOVEMENT_UTILS_H

