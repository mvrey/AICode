#pragma once

#include "config.h"
#include "ecs/Entity.h"
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

/// Focuses the camera on a world position
/// @param world_pos The world position to focus on
void FocusOn(const ::MOMOS::Vec2& world_pos);

/// Starts following an entity (camera will track its position)
/// @param entity The entity to follow
void StartFollowing(ECS::Entity entity);

/// Stops following any entity
void StopFollowing();

/// Updates camera follow behavior (should be called each frame)
/// @param delta_seconds Time since last frame in seconds
void UpdateFollow(float delta_seconds);

/// Checks if camera is currently following an entity
bool IsFollowing();

/// Gets the entity the camera is currently following
/// @return The entity being followed, or invalid entity if not following
ECS::Entity GetFollowingEntity();

} // namespace Camera

