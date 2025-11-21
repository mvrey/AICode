#include "../include/VSyncToggle.h"

#include <MOMOS/momos.h>
#include <MOMOS/draw.h>
#include <GLFW/glfw3.h>

#include <cstring>

VSyncToggle::VSyncToggle()
	: vsync_enabled_(false) {
	button_bounds_[0] = button_bounds_[1] = button_bounds_[2] = button_bounds_[3] = 0.0f;
}

void VSyncToggle::Initialize(bool enabled) {
	vsync_enabled_ = enabled;
	Apply();
}

void VSyncToggle::Apply() const {
	if (MOMOS::win != nullptr) {
		glfwMakeContextCurrent(MOMOS::win);
		glfwSwapInterval(vsync_enabled_ ? 1 : 0);
	}
}

void VSyncToggle::Draw(float anchor_x, float anchor_baseline_y) const {
	float button_x = anchor_x + kButtonOffsetX;
	float button_y = anchor_baseline_y - kButtonOffsetY;

	button_bounds_[0] = button_x;
	button_bounds_[1] = button_y;
	button_bounds_[2] = button_x + kButtonWidth;
	button_bounds_[3] = button_y + kButtonHeight;

	float points[10] = {
		button_bounds_[0], button_bounds_[1],
		button_bounds_[2], button_bounds_[1],
		button_bounds_[2], button_bounds_[3],
		button_bounds_[0], button_bounds_[3],
		button_bounds_[0], button_bounds_[1]
	};

	unsigned char baseColor = vsync_enabled_ ? 90 : 60;
	unsigned char alpha = vsync_enabled_ ? 200 : 140;

	MOMOS::DrawSetFillColor(baseColor, baseColor, baseColor, alpha);
	MOMOS::DrawSolidPath(points, 5);
	MOMOS::DrawSetStrokeColor(220, 220, 220, 180);
	MOMOS::DrawPath(points, 5);

	const char* label = vsync_enabled_ ? "VSYNC ON" : "VSYNC OFF";
	MOMOS::DrawSetFillColor(240, 240, 240, 255);
	MOMOS::DrawSetTextSize(14.0f);
	MOMOS::DrawText(button_x + 8.0f, button_y + kButtonHeight / 2.0f + 5.0f, label);
}

bool VSyncToggle::PointInside(float x, float y) const {
	return x >= button_bounds_[0] && x <= button_bounds_[2] &&
		y >= button_bounds_[1] && y <= button_bounds_[3];
}

bool VSyncToggle::HandleClick(float mouse_x, float mouse_y) {
	if (PointInside(mouse_x, mouse_y)) {
		vsync_enabled_ = !vsync_enabled_;
		Apply();
		return true;
	}
	return false;
}

