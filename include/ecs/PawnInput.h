//------------------------------------------------------------------------------
// File: PawnInput.h
// Purpose: Declares helper functions that expose MOMOS input state to ECS code
//          without pulling UI headers everywhere.
//------------------------------------------------------------------------------
#ifndef PAWN_ECS_INPUT_H
#define PAWN_ECS_INPUT_H

#include <MOMOS/math.h>

namespace PawnECS {

// Returns the current mouse cursor position in screen space.
MOMOS::Vec2 GetMouseScreenPosition();
// True while the primary mouse button is pressed.
bool IsPrimaryMouseDown();

} // namespace PawnECS

#endif // PAWN_ECS_INPUT_H

