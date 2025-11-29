//------------------------------------------------------------------------------
// File: NeedsControllerComponent.h
// Purpose: Component that stores a pawn's NeedsController
//------------------------------------------------------------------------------
#ifndef ECS_NEEDS_CONTROLLER_COMPONENT_H
#define ECS_NEEDS_CONTROLLER_COMPONENT_H

#include "../../Needs/NeedsController.h"
#include <memory>

namespace ECS {

/// Component that stores a pawn's NeedsController
/// Allows the FSM to access and restore needs
struct NeedsControllerComponent {
	std::unique_ptr<NeedsController> controller;
};

} // namespace ECS

#endif // ECS_NEEDS_CONTROLLER_COMPONENT_H

