//------------------------------------------------------------------------------
// File: Need.cc
// Purpose: Implementation of Need class
//------------------------------------------------------------------------------
#include "../../include/Needs/Need.h"
#include <algorithm>

Need::Need(NeedId id, float initial_value, float decay_rate, float threshold)
	: id_(id)
	, value_(std::max(0.0f, std::min(1.0f, initial_value)))
	, decay_rate_(decay_rate)
	, threshold_(std::max(0.0f, std::min(1.0f, threshold)))
{
}

void Need::Tick(double delta_time) {
	// delta_time is in milliseconds, convert to seconds
	float delta_seconds = static_cast<float>(delta_time) / 1000.0f;
	value_ -= decay_rate_ * delta_seconds;
	value_ = std::max(0.0f, std::min(1.0f, value_));
}

void Need::Restore(float amount) {
	value_ += amount;
	value_ = std::max(0.0f, std::min(1.0f, value_));
}

bool Need::IsUrgent() const {
	return value_ < threshold_;
}

