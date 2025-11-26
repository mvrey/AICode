#include "../../include/Camera.h"

#include <cmath>

namespace {

constexpr float kMinZoom = 1.0f;
constexpr float kMaxZoom = 3.5f;
constexpr float kPanZoomThreshold = 1.001f;

::MOMOS::Vec2 g_world_size = { static_cast<float>(Screen::width), static_cast<float>(Screen::height) };
::MOMOS::Vec2 g_center = { g_world_size.x * 0.5f, g_world_size.y * 0.5f };
float g_zoom = kMinZoom;

::MOMOS::Vec2 ScreenCenter() {
	return ::MOMOS::Vec2{
		static_cast<float>(Screen::width) * 0.5f,
		static_cast<float>(Screen::height) * 0.5f
	};
}

void ClampCenter() {
	const float half_view_x = (static_cast<float>(Screen::width) / g_zoom) * 0.5f;
	const float half_view_y = (static_cast<float>(Screen::height) / g_zoom) * 0.5f;

	const float min_center_x = half_view_x;
	const float max_center_x = g_world_size.x - half_view_x;
	if (max_center_x <= min_center_x) {
		g_center.x = g_world_size.x * 0.5f;
	} else {
		if (g_center.x < min_center_x) {
			g_center.x = min_center_x;
		}
		if (g_center.x > max_center_x) {
			g_center.x = max_center_x;
		}
	}

	const float min_center_y = half_view_y;
	const float max_center_y = g_world_size.y - half_view_y;
	if (max_center_y <= min_center_y) {
		g_center.y = g_world_size.y * 0.5f;
	} else {
		if (g_center.y < min_center_y) {
			g_center.y = min_center_y;
		}
		if (g_center.y > max_center_y) {
			g_center.y = max_center_y;
		}
	}
}

} // namespace

namespace Camera {

void Initialize() {
	g_center = { g_world_size.x * 0.5f, g_world_size.y * 0.5f };
	g_zoom = kMinZoom;
	ClampCenter();
}

float Zoom() {
	return g_zoom;
}

bool CanPan() {
	return g_zoom > kPanZoomThreshold;
}

::MOMOS::Vec2 WorldToScreen(const ::MOMOS::Vec2& world) {
	const ::MOMOS::Vec2 center_screen = ScreenCenter();
	return ::MOMOS::Vec2{
		(world.x - g_center.x) * g_zoom + center_screen.x,
		(world.y - g_center.y) * g_zoom + center_screen.y
	};
}

::MOMOS::Vec2 ScreenToWorld(const ::MOMOS::Vec2& screen) {
	const ::MOMOS::Vec2 center_screen = ScreenCenter();
	return ::MOMOS::Vec2{
		(screen.x - center_screen.x) / g_zoom + g_center.x,
		(screen.y - center_screen.y) / g_zoom + g_center.y
	};
}

void ZoomBy(float delta, const ::MOMOS::Vec2& focus_screen) {
	if (delta == 0.0f) {
		return;
	}

	const ::MOMOS::Vec2 focus_before = ScreenToWorld(focus_screen);
	float new_zoom = g_zoom + delta;
	if (new_zoom < kMinZoom) {
		new_zoom = kMinZoom;
	} else if (new_zoom > kMaxZoom) {
		new_zoom = kMaxZoom;
	}

	if (new_zoom == g_zoom) {
		return;
	}

	g_zoom = new_zoom;
	ClampCenter();

	const ::MOMOS::Vec2 focus_after = ScreenToWorld(focus_screen);
	g_center.x += focus_before.x - focus_after.x;
	g_center.y += focus_before.y - focus_after.y;
	ClampCenter();
}

void Pan(const ::MOMOS::Vec2& delta) {
	if (!CanPan() || (delta.x == 0.0f && delta.y == 0.0f)) {
		return;
	}

	g_center.x += delta.x;
	g_center.y += delta.y;
	ClampCenter();
}

} // namespace Camera

