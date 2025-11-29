#ifndef SIMULATION_SPEED_CONTROLS_H
#define SIMULATION_SPEED_CONTROLS_H

#include <functional>

class SimulationSpeedControls {
public:
	static constexpr int kButtonCount = 3;

	SimulationSpeedControls();

	void Initialize();
	void HandleInput();
	void Draw() const;
	
	/// Sets callback to be called when speed changes
	/// @param callback Function that receives the new speed multiplier
	void SetSpeedChangedCallback(std::function<void(float)> callback);
	
	/// Checks if a click at the given coordinates is on any speed control button
	/// @param x Mouse X coordinate
	/// @param y Mouse Y coordinate
	/// @return true if click is on a button, false otherwise
	bool IsClickOnControls(float x, float y) const;

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
	
	// Keyboard state tracking
	bool minus_key_was_pressed_;
	bool plus_key_was_pressed_;
	
	// Callback for speed changes
	std::function<void(float)> on_speed_changed_;
};

#endif

