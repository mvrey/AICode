//------------------------------------------------------------------------------
// File: PawnMovementUtils.h
// Purpose: Declares helper routines for manipulating ECS movement state without
//          touching legacy Pawn objects.
//------------------------------------------------------------------------------
#ifndef PAWN_ECS_MOVEMENT_UTILS_H
#define PAWN_ECS_MOVEMENT_UTILS_H

#include <vector>
#include "./Registry.h"
#include "../Map/Map.h"
#include <MOMOS/math.h>
#include "components/MovementComponent.h"
#include "components/TransformComponent.h"

struct GameContext;

namespace PawnECS {
namespace MovementUtils {

enum class PathFinalizationResult {
	kNotReady,
	kSuccess,
	kFailure,
};

void ClearMovement(ECS::Registry& registry, ECS::Entity entity);
PathFinalizationResult TryFinalizePath(ECS::Registry& registry, ECS::Entity entity, const GameContext* context);
bool RequestPathTo(ECS::Registry& registry, ECS::Entity entity, const ::MOMOS::Vec2& destination, const GameContext* context);
void SetDoorRouteActive(ECS::Registry& registry, ECS::Entity entity, bool active);
void SetEscapeRouteActive(ECS::Registry& registry, ECS::Entity entity, bool active);
void CycleDoorTarget(ECS::Registry& registry, ECS::Entity entity, int totalDoors);
void SetDoorTarget(ECS::Registry& registry, ECS::Entity entity, int target);

} // namespace MovementUtils
} // namespace PawnECS

#endif // PAWN_ECS_MOVEMENT_UTILS_H

