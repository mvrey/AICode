#include "../include/FpsCounter.h"

#include <MOMOS/momos.h>
#include <MOMOS/draw.h>
#include <MOMOS/time.h>
#include "../include/config.h"

#include <cstring>
#include <cstdio>

FpsCounter::FpsCounter()
	: next_update_ms_(0.0)
	, last_frame_count_(0)
	, cached_fps_(0.0)
	, last_text_right_(0.0f)
	, last_text_baseline_y_(18.0f) {
}

void FpsCounter::Update() {
	double now = MOMOS::Time();
	if (now >= next_update_ms_) {
		unsigned long long total_frames = MOMOS::frame_counter;
		unsigned long long diff = total_frames - last_frame_count_;
		double interval_seconds = update_interval_ms_ / 1000.0;
		cached_fps_ = static_cast<double>(diff) / interval_seconds;
		last_frame_count_ = total_frames;
		next_update_ms_ = now + update_interval_ms_;
	}
}

void FpsCounter::Draw() const {
	char buffer[64];
	std::snprintf(buffer, sizeof(buffer), "FPS: %.1f", cached_fps_);

	float text_width_estimate = static_cast<float>(std::strlen(buffer)) * 8.0f;
	float text_x = (Screen::width - text_width_estimate) * 0.5f;
	float text_y = 18.0f;

	MOMOS::DrawSetFillColor(240, 240, 240, 255);
	MOMOS::DrawSetTextSize(18.0f);
	MOMOS::DrawText(text_x, text_y, buffer);

	last_text_right_ = text_x + text_width_estimate;
	last_text_baseline_y_ = text_y;
}

