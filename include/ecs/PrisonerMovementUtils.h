//------------------------------------------------------------------------------
// File: PrisonerMovementUtils.h
// Purpose: Declares helper functions that bridge legacy Prisoner instances with
//          ECS movement components, ensuring both worlds stay in sync.
//------------------------------------------------------------------------------
#ifndef PRISONER_ECS_MOVEMENT_UTILS_H
#define PRISONER_ECS_MOVEMENT_UTILS_H

#include <MOMOS/math.h>

class Prisoner;

namespace PrisonerECS {
namespace MovementUtils {

// Requests a deterministic path toward the destination and mirrors it into ECS.
bool SetPathTo(Prisoner& prisoner, const ::MOMOS::Vec2& destination);
// Steps the entity along its current path, updating transforms and completion.
bool MoveFollowingPath(Prisoner& prisoner);
// Resets ECS/legacy movement bookkeeping when routes finish or fail.
void ClearMovement(Prisoner& prisoner);

} // namespace MovementUtils
} // namespace PrisonerECS

#endif // PRISONER_ECS_MOVEMENT_UTILS_H

