#ifndef PRISONER_ECS_INPUT_H
#define PRISONER_ECS_INPUT_H

#include <MOMOS/math.h>

namespace PrisonerECS {

MOMOS::Vec2 GetMouseScreenPosition();
bool IsPrimaryMouseDown();

} // namespace PrisonerECS

#endif // PRISONER_ECS_INPUT_H

