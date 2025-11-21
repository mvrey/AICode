#include "../../include/ecs/PrisonerInput.h"
#include "../../include/GameStatus.h"
#include <MOMOS/input.h>

namespace PrisonerECS {

MOMOS::Vec2 GetMouseScreenPosition() {
	return MOMOS::Vec2{
		static_cast<float>(MOMOS::MousePositionX()),
		static_cast<float>(MOMOS::MousePositionY())
	};
}

bool IsPrimaryMouseDown() {
	return MOMOS::MouseButtonDown(1);
}

} // namespace PrisonerECS

