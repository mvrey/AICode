#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

namespace ECS {

class Registry;

class IEcsSystem {
public:
	virtual ~IEcsSystem() = default;
	virtual void Update(Registry& registry, double delta_time) = 0;
};

} // namespace ECS

#endif // ECS_SYSTEM_H

