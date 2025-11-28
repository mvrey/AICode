#pragma once

#include "config.h"
#include <MOMOS/math.h>

namespace Camera {

// Camera constants
constexpr float kZoomStep = 0.25f;

void Initialize();
float Zoom();
bool CanPan();

::MOMOS::Vec2 WorldToScreen(const ::MOMOS::Vec2& world);
::MOMOS::Vec2 ScreenToWorld(const ::MOMOS::Vec2& screen);

void ZoomBy(float delta, const ::MOMOS::Vec2& focus_screen);
void Pan(const ::MOMOS::Vec2& delta);

/// Handles camera input (zoom and pan) from keyboard, mouse, and edge panning
/// @param delta_seconds Time since last frame in seconds
void HandleInput(float delta_seconds);

} // namespace Camera

