#ifndef ECS_PRISONER_MOVEMENT_SYSTEM_H
#define ECS_PRISONER_MOVEMENT_SYSTEM_H

#include "../System.h"

namespace ECS {

class PrisonerMovementSystem : public IEcsSystem {
public:
	void Update(Registry& registry, double delta_time) override;
};

} // namespace ECS

#endif // ECS_PRISONER_MOVEMENT_SYSTEM_H

