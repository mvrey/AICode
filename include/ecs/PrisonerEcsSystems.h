#ifndef PRISONER_ECS_SYSTEMS_H
#define PRISONER_ECS_SYSTEMS_H

#include "Registry.h"

namespace ECS {
class PrisonerAISystem;
class PrisonerMovementSystem;
class PrisonerRenderSystem;
} // namespace ECS

namespace PrisonerECS {

class Systems {
public:
	static Systems& Get();

	void Update(double delta_time);
	void Render(double delta_time);

private:
	Systems() = default;

	ECS::PrisonerAISystem* ai_system_ = nullptr;
	ECS::PrisonerMovementSystem* movement_system_ = nullptr;
	ECS::PrisonerRenderSystem* render_system_ = nullptr;
};

} // namespace PrisonerECS

#endif // PRISONER_ECS_SYSTEMS_H

