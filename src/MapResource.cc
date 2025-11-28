#include "../include/MapResource.h"
#include <cstdlib>
#include <algorithm>

MapResource::MapResource() {
}

MapResource::MapResource(const MapResourceType* type, int amount)
	: type(type), amount(amount) {
}

MapResource::~MapResource() {
}

bool MapResource::CanUse() const {
	return type != nullptr && amount > 0;
}

std::vector<MapResource> MapResource::Use() const {
	std::vector<MapResource> outputs;
	
	if (!CanUse()) {
		return outputs;
	}

	// Generate outputs based on the resource type's output definitions
	for (const auto& output_pair : type->resource_outputs) {
		const MapResourceType* output_type = output_pair.first;
		const auto& range = output_pair.second;
		
		// Generate random amount within range [min, max]
		int min_amount = range.first;
		int max_amount = range.second;
		int output_amount = min_amount;
		if (max_amount > min_amount) {
			output_amount = min_amount + (rand() % (max_amount - min_amount + 1));
		}
		
		if (output_amount > 0 && output_type != nullptr) {
			outputs.emplace_back(output_type, output_amount);
		}
	}

	return outputs;
}

