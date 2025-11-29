//------------------------------------------------------------------------------
// File: NeedsController.h
// Purpose: Manages all needs for a pawn and provides the most urgent one
//------------------------------------------------------------------------------
#ifndef NEEDS_CONTROLLER_H
#define NEEDS_CONTROLLER_H

#include "INeed.h"
#include "NeedId.h"
#include <vector>
#include <memory>

/// Manages all needs for a pawn and determines which is most urgent
class NeedsController {
public:
	NeedsController();
	~NeedsController();

	/// Add a need to be managed
	void AddNeed(std::unique_ptr<INeed> need);

	/// Get a need by its ID (returns nullptr if not found)
	INeed* GetNeed(NeedId id);

	/// Get the most urgent need (lowest value below threshold, or lowest overall)
	/// Returns nullptr if no needs are urgent
	INeed* GetMostUrgentNeed() const;

	/// Update all needs with decay over delta_time
	void Tick(double delta_time);

	/// Get all needs (for iteration)
	const std::vector<std::unique_ptr<INeed>>& GetAllNeeds() const { return needs_; }

private:
	std::vector<std::unique_ptr<INeed>> needs_;
};

#endif // NEEDS_CONTROLLER_H

