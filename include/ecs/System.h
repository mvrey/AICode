//------------------------------------------------------------------------------
// File: System.h
// Purpose: Defines the common interface implemented by every ECS system so the
//          game loop can update them uniformly.
//------------------------------------------------------------------------------
#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

namespace ECS {

class Registry;

class IEcsSystem {
public:
	// Virtual destructor ensures derived systems clean up properly.
	virtual ~IEcsSystem() = default;
	// Core update hook invoked once per frame with the ECS registry.
	virtual void Update(Registry& registry, double delta_time) = 0;
};

} // namespace ECS

#endif // ECS_SYSTEM_H

