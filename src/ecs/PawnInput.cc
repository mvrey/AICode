//------------------------------------------------------------------------------
// File: PawnInput.cc
// Purpose: Implements lightweight wrappers that expose MOMOS input state to ECS
//          systems without leaking engine headers everywhere.
//------------------------------------------------------------------------------
#include "../../include/ecs/PawnInput.h"
#include "../../include/GameStatus.h"
#include <MOMOS/input.h>

namespace PawnECS {

// Converts MOMOS' integer cursor position into a float vector.
MOMOS::Vec2 GetMouseScreenPosition() {
	return MOMOS::Vec2{
		static_cast<float>(MOMOS::MousePositionX()),
		static_cast<float>(MOMOS::MousePositionY())
	};
}

// Returns true while the left mouse button is pressed.
bool IsPrimaryMouseDown() {
	return MOMOS::MouseButtonDown(1);
}

} // namespace PawnECS

