#ifndef FPS_COUNTER_H
#define FPS_COUNTER_H

class FpsCounter {
public:
	FpsCounter();

	void Update();
	void Draw() const;

private:
	const double update_interval_ms_ = 500.0;
	double next_update_ms_;
	unsigned long long last_frame_count_;
	double cached_fps_;
};

#endif

