//------------------------------------------------------------------------------
// File: PrisonerInput.h
// Purpose: Declares helper functions that expose MOMOS input state to ECS code
//          without pulling UI headers everywhere.
//------------------------------------------------------------------------------
#ifndef PRISONER_ECS_INPUT_H
#define PRISONER_ECS_INPUT_H

#include <MOMOS/math.h>

namespace PrisonerECS {

// Returns the current mouse cursor position in screen space.
MOMOS::Vec2 GetMouseScreenPosition();
// True while the primary mouse button is pressed.
bool IsPrimaryMouseDown();

} // namespace PrisonerECS

#endif // PRISONER_ECS_INPUT_H

