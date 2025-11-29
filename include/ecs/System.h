//------------------------------------------------------------------------------
// File: System.h
// Purpose: Defines the common interface implemented by every ECS system so the
//          game loop can update them uniformly.
//------------------------------------------------------------------------------
#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

#include "../Core/GameContext.h"

namespace ECS {
	class Registry;
}

class IEcsSystem {
public:
	// Virtual destructor ensures derived systems clean up properly.
	virtual ~IEcsSystem() = default;
	// Core update hook invoked once per frame with the ECS registry.
	// Context may be nullptr for systems that don't need it.
	virtual void Update(ECS::Registry& registry, double delta_time, const GameContext* context = nullptr) = 0;
};

#endif // ECS_SYSTEM_H

