#ifndef ECS_PRISONER_PATH_FOLLOW_SYSTEM_H
#define ECS_PRISONER_PATH_FOLLOW_SYSTEM_H

#include "../System.h"

namespace ECS {

class PrisonerPathFollowSystem : public IEcsSystem {
public:
	void Update(Registry& registry, double delta_time) override;
};

} // namespace ECS

#endif // ECS_PRISONER_PATH_FOLLOW_SYSTEM_H

