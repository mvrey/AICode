#pragma once

#include "config.h"
#include <MOMOS/math.h>

namespace Camera {

void Initialize();
float Zoom();
bool CanPan();

::MOMOS::Vec2 WorldToScreen(const ::MOMOS::Vec2& world);
::MOMOS::Vec2 ScreenToWorld(const ::MOMOS::Vec2& screen);

void ZoomBy(float delta, const ::MOMOS::Vec2& focus_screen);
void Pan(const ::MOMOS::Vec2& delta);

} // namespace Camera

