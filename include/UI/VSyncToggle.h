#ifndef VSYNC_TOGGLE_H
#define VSYNC_TOGGLE_H

class VSyncToggle {
public:
	VSyncToggle();

	void Initialize(bool enabled);
	void Draw(float anchor_x, float anchor_baseline_y) const;
	bool HandleClick(float mouse_x, float mouse_y);
	bool IsEnabled() const { return vsync_enabled_; }

private:
	void Apply() const;
	bool PointInside(float x, float y) const;

	bool vsync_enabled_;
	mutable float button_bounds_[4];

	static constexpr float kButtonWidth = 90.0f;
	static constexpr float kButtonHeight = 24.0f;
	static constexpr float kButtonOffsetX = 12.0f;
	static constexpr float kButtonOffsetY = 12.0f;
};

#endif

