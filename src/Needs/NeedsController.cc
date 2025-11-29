//------------------------------------------------------------------------------
// File: NeedsController.cc
// Purpose: Implementation of NeedsController
//------------------------------------------------------------------------------
#include "../../include/Needs/NeedsController.h"
#include <algorithm>

NeedsController::NeedsController() {
}

NeedsController::~NeedsController() {
}

void NeedsController::AddNeed(std::unique_ptr<INeed> need) {
	if (need) {
		needs_.push_back(std::move(need));
	}
}

INeed* NeedsController::GetNeed(NeedId id) {
	for (auto& need : needs_) {
		if (need && need->GetId() == id) {
			return need.get();
		}
	}
	return nullptr;
}

INeed* NeedsController::GetMostUrgentNeed() const {
	INeed* most_urgent = nullptr;
	float lowest_value = 1.0f;

	// First, find the most urgent need (below threshold)
	for (const auto& need : needs_) {
		if (need && need->IsUrgent()) {
			float value = need->GetValue();
			if (value < lowest_value) {
				lowest_value = value;
				most_urgent = need.get();
			}
		}
	}

	// If no urgent needs, return the one with lowest value overall
	if (most_urgent == nullptr) {
		for (const auto& need : needs_) {
			if (need) {
				float value = need->GetValue();
				if (value < lowest_value) {
					lowest_value = value;
					most_urgent = need.get();
				}
			}
		}
	}

	return most_urgent;
}

void NeedsController::Tick(double delta_time) {
	for (auto& need : needs_) {
		if (need) {
			need->Tick(delta_time);
		}
	}
}

