//------------------------------------------------------------------------------
// File: INeed.h
// Purpose: Interface for individual needs that can decay and be restored
//------------------------------------------------------------------------------
#ifndef I_NEED_H
#define I_NEED_H

#include "NeedId.h"

/// Interface for a single need that can decay over time and be restored
class INeed {
public:
	virtual ~INeed() = default;

	/// Get the unique identifier for this need
	virtual NeedId GetId() const = 0;

	/// Get the current value of this need (0.0 to 1.0)
	virtual float GetValue() const = 0;

	/// Get the decay rate per second
	virtual float GetDecayRate() const = 0;

	/// Get the threshold below which this need becomes urgent
	virtual float GetThreshold() const = 0;

	/// Update the need by applying decay over delta_time seconds
	virtual void Tick(double delta_time) = 0;

	/// Restore the need by the specified amount (clamped to [0, 1])
	virtual void Restore(float amount) = 0;

	/// Check if this need is below its threshold (urgent)
	virtual bool IsUrgent() const = 0;
};

#endif // I_NEED_H

