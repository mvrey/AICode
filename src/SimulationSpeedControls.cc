#include "../include/SimulationSpeedControls.h"

#include "../include/GameStatus.h"
#include "../include/config.h"

#include <MOMOS/draw.h>
#include <MOMOS/input.h>

#include <algorithm>
#include <cstdio>

namespace {
	const char* kButtonLabels[SimulationSpeedControls::kButtonCount] = { "-", "||", "+" };
}

const SimulationSpeedControls::SpeedControlOption SimulationSpeedControls::kOptions[kOptionCount] = {
	{ "Paused", 0.0 },
	{ "1x", 1.0 },
	{ "2x", 2.0 },
	{ "3x", 3.0 },
	{ "4x", 4.0 },
	{ "5x", 5.0 },
};

SimulationSpeedControls::SimulationSpeedControls()
	: speed_index_(1)
	, last_nonzero_speed_index_(1)
	, hovered_button_(-1) {
}

void SimulationSpeedControls::Initialize() {
	speed_index_ = 1;
	last_nonzero_speed_index_ = 1;
	hovered_button_ = -1;
	ApplySpeedIndex(speed_index_);
}

void SimulationSpeedControls::HandleInput() {
	float mx = static_cast<float>(MOMOS::MousePositionX());
	float my = static_cast<float>(MOMOS::MousePositionY());
	hovered_button_ = ButtonIndexAt(mx, my);

	if (hovered_button_ != -1 && MOMOS::MouseButtonDown(1) && !ButtonDisabled(hovered_button_)) {
		switch (hovered_button_) {
		case 0:
			DecreaseSpeed();
			break;
		case 1:
			ToggleSpeed();
			break;
		case 2:
			IncreaseSpeed();
			break;
		}
	}
}

void SimulationSpeedControls::Draw() const {
	const SpeedControlOption& current = CurrentOption();

	char label_text[32];
	if (current.multiplier == 0.0) {
		std::snprintf(label_text, sizeof(label_text), "Speed: Paused");
	} else {
		std::snprintf(label_text, sizeof(label_text), "Speed: %s", current.label);
	}

	MOMOS::DrawSetFillColor(240, 240, 240, 255);
	MOMOS::DrawSetTextSize(kTextSize);
	MOMOS::DrawText(ControlsBaseX(), kTopPadding + kTextSize, label_text);

	for (int i = 0; i < kButtonCount; ++i) {
		float bx = ButtonX(i);
		float by = ButtonsY();

		bool disabled = ButtonDisabled(i);

		unsigned char baseColor = disabled ? 40 : 65;
		unsigned char alpha = disabled ? 90 : 150;

		if (i == hovered_button_ && !disabled) {
			baseColor = 85;
			alpha = 200;
		}

		float points[10] = {
			bx, by,
			bx + kButtonWidth, by,
			bx + kButtonWidth, by + kButtonHeight,
			bx, by + kButtonHeight,
			bx, by
		};

		MOMOS::DrawSetFillColor(baseColor, baseColor, baseColor, alpha);
		MOMOS::DrawSolidPath(points, 5);
		MOMOS::DrawSetStrokeColor(220, 220, 220, 180);
		MOMOS::DrawPath(points, 5);

		const char* button_label = kButtonLabels[i];
		if (i == 1 && current.multiplier == 0.0) {
			button_label = ">";
		}

		MOMOS::DrawSetFillColor(240, 240, 240, disabled ? 120 : 255);
		MOMOS::DrawSetTextSize(14.0f);
		float text_x = bx + kButtonWidth / 2.0f - 6.0f;
		float text_y = by + kButtonHeight / 2.0f + 5.0f;
		MOMOS::DrawText(text_x, text_y, button_label);
	}
}

double SimulationSpeedControls::CurrentMultiplier() const {
	return CurrentOption().multiplier;
}

float SimulationSpeedControls::ControlsTotalWidth() const {
	return kButtonCount * kButtonWidth + (kButtonCount - 1) * kButtonSpacing;
}

float SimulationSpeedControls::ControlsBaseX() const {
	return Screen::width - kRightPadding - ControlsTotalWidth();
}

float SimulationSpeedControls::ButtonsY() const {
	return kTopPadding + kTextSize + 6.0f;
}

float SimulationSpeedControls::ButtonX(int button_index) const {
	return ControlsBaseX() + button_index * (kButtonWidth + kButtonSpacing);
}

bool SimulationSpeedControls::PointInsideButton(int button_index, float x, float y) const {
	float bx = ButtonX(button_index);
	float by = ButtonsY();
	return (x >= bx && x <= bx + kButtonWidth && y >= by && y <= by + kButtonHeight);
}

int SimulationSpeedControls::ButtonIndexAt(float x, float y) const {
	for (int i = 0; i < kButtonCount; ++i) {
		if (PointInsideButton(i, x, y)) {
			return i;
		}
	}
	return -1;
}

void SimulationSpeedControls::ApplySpeedIndex(int index) {
	index = std::max(0, std::min(kOptionCount - 1, index));
	speed_index_ = index;
	GameStatus::get()->simulation_speed_ = static_cast<float>(kOptions[index].multiplier);

	if (kOptions[index].multiplier > 0.0) {
		last_nonzero_speed_index_ = index;
	}
}

void SimulationSpeedControls::IncreaseSpeed() {
	if (speed_index_ < kOptionCount - 1) {
		ApplySpeedIndex(speed_index_ + 1);
	}
}

void SimulationSpeedControls::DecreaseSpeed() {
	if (speed_index_ > 0) {
		ApplySpeedIndex(speed_index_ - 1);
	}
}

void SimulationSpeedControls::ToggleSpeed() {
	if (kOptions[speed_index_].multiplier == 0.0) {
		int target = (last_nonzero_speed_index_ > 0) ? last_nonzero_speed_index_ : 1;
		ApplySpeedIndex(target);
	} else {
		last_nonzero_speed_index_ = speed_index_;
		ApplySpeedIndex(0);
	}
}

bool SimulationSpeedControls::ButtonDisabled(int button_index) const {
	if (button_index == 0) {
		return speed_index_ == 0;
	}
	if (button_index == 2) {
		return speed_index_ >= kOptionCount - 1 && CurrentOption().multiplier != 0.0;
	}
	return false;
}

const SimulationSpeedControls::SpeedControlOption& SimulationSpeedControls::CurrentOption() const {
	return kOptions[speed_index_];
}

