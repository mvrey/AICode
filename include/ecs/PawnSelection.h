#ifndef PAWN_SELECTION_H
#define PAWN_SELECTION_H

#include "Entity.h"
#include "../config.h"

namespace PawnSelection {

/// Handles clicking on pawns and returns true if a pawn was clicked
/// @return true if a pawn was clicked, false otherwise
bool HandleClick();

/// Draws selection box around the currently selected pawn
void DrawSelection();

/// Gets the currently selected pawn entity
/// @return The selected pawn entity, or invalid entity if none selected
ECS::Entity GetSelectedPawn();

/// Clears the current pawn selection
void ClearSelection();

/// Handles keyboard input for pawn navigation
/// @return true if a pawn was selected, false otherwise
bool HandleKeyboardNavigation();

} // namespace PawnSelection

#endif // PAWN_SELECTION_H

