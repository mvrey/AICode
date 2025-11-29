//------------------------------------------------------------------------------
// File: INeedProvider.h
// Purpose: Interface for world objects that can satisfy pawn needs
//------------------------------------------------------------------------------
#ifndef I_NEED_PROVIDER_H
#define I_NEED_PROVIDER_H

#include "../Needs/NeedId.h"
#include <MOMOS/math.h>

/// Interface for world objects that can restore a specific need
class INeedProvider {
public:
	virtual ~INeedProvider() = default;

	/// Get the need ID that this provider satisfies
	virtual NeedId GetNeedId() const = 0;

	/// Get the amount this provider restores (0.0 to 1.0)
	virtual float GetRestoreAmount() const = 0;

	/// Get the duration in seconds required to use this provider
	virtual double GetUseDuration() const = 0;

	/// Check if this provider is currently available for use
	virtual bool IsAvailable() const = 0;

	/// Called when a pawn finishes using this provider
	/// This is where the provider performs its effect (e.g., plant disappears)
	virtual void OnUsed() = 0;

	/// Get the world position of this provider
	virtual ::MOMOS::Vec2 GetPosition() const = 0;
};

#endif // I_NEED_PROVIDER_H

