//------------------------------------------------------------------------------
// File: Need.h
// Purpose: Concrete implementation of INeed interface
//------------------------------------------------------------------------------
#ifndef NEED_H
#define NEED_H

#include "INeed.h"

/// Concrete implementation of a need
class Need : public INeed {
public:
	Need(NeedId id, float initial_value, float decay_rate, float threshold);

	NeedId GetId() const override { return id_; }
	float GetValue() const override { return value_; }
	float GetDecayRate() const override { return decay_rate_; }
	float GetThreshold() const override { return threshold_; }

	void Tick(double delta_time) override;
	void Restore(float amount) override;
	bool IsUrgent() const override;

private:
	NeedId id_;
	float value_;          // Current value [0.0, 1.0]
	float decay_rate_;     // Decay per second
	float threshold_;      // Urgency threshold [0.0, 1.0]
};

#endif // NEED_H

