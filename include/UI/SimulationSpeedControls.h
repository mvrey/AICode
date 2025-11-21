#ifndef SIMULATION_SPEED_CONTROLS_H
#define SIMULATION_SPEED_CONTROLS_H

class SimulationSpeedControls {
public:
	static constexpr int kButtonCount = 3;

	SimulationSpeedControls();

	void Initialize();
	void HandleInput();
	void Draw() const;

	double CurrentMultiplier() const;

private:
	struct SpeedControlOption {
		const char* label;
		double multiplier;
	};

	static constexpr int kOptionCount = 6;
	static const SpeedControlOption kOptions[kOptionCount];

	static constexpr float kRightPadding = 16.0f;
	static constexpr float kTopPadding = 16.0f;
	static constexpr float kButtonWidth = 32.0f;
	static constexpr float kButtonHeight = 26.0f;
	static constexpr float kButtonSpacing = 4.0f;
	static constexpr float kTextSize = 16.0f;

	float ControlsTotalWidth() const;
	float ControlsBaseX() const;
	float ButtonsY() const;
	float ButtonX(int button_index) const;

	bool PointInsideButton(int button_index, float x, float y) const;
	int ButtonIndexAt(float x, float y) const;

	void ApplySpeedIndex(int index);
	void IncreaseSpeed();
	void DecreaseSpeed();
	void ToggleSpeed();

	bool ButtonDisabled(int button_index) const;
	const SpeedControlOption& CurrentOption() const;

	int speed_index_;
	int last_nonzero_speed_index_;
	int hovered_button_;
};

#endif

