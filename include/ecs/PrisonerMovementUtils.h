#ifndef PRISONER_ECS_MOVEMENT_UTILS_H
#define PRISONER_ECS_MOVEMENT_UTILS_H

#include <MOMOS/math.h>

class Prisoner;

namespace PrisonerECS {
namespace MovementUtils {

bool SetPathTo(Prisoner& prisoner, const ::MOMOS::Vec2& destination);
bool MoveFollowingPath(Prisoner& prisoner);
void ClearMovement(Prisoner& prisoner);

void SetDoorRouteActive(Prisoner& prisoner, bool active);
void CycleDoorTarget(Prisoner& prisoner, int totalDoors);
void SetDoorTarget(Prisoner& prisoner, int doorIndex);
void SetEscapeRouteActive(Prisoner& prisoner, bool active);

} // namespace MovementUtils
} // namespace PrisonerECS

#endif // PRISONER_ECS_MOVEMENT_UTILS_H

