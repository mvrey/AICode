//------------------------------------------------------------------------------
// File: ResourceProviderRegistry.cc
// Purpose: Implementation of ResourceProviderRegistry
//------------------------------------------------------------------------------
#include "../../include/Providers/ResourceProviderRegistry.h"
#include "../../include/Providers/ProviderRegistry.h"
#include "../../include/Providers/MapResourceProvider.h"
#include "../../include/Providers/ResourceNeedMapping.h"
#include "../../include/Pathfinding/cost_map.h"
#include "../../include/Map/MapCell.h"
#include "../../include/Map/MapResource.h"
#include <memory>

ResourceProviderRegistry& ResourceProviderRegistry::Get() {
	static ResourceProviderRegistry instance;
	return instance;
}

void ResourceProviderRegistry::RegisterMapResources(CostMap& cost_map) {
	// Clear existing resource providers first
	Clear();

	ResourceNeedMapping& mapping = ResourceNeedMapping::Get();
	ProviderRegistry& provider_registry = ProviderRegistry::Get();

	Map& map = cost_map.GetMap();
	int width = map.getWidth();
	int height = map.getHeight();

	// Scan all cells and register resources as providers
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			MapCell* cell = map.getCellAt(x, y);
			if (!cell) {
				continue;
			}

			// Check each resource in the cell
			for (auto& resource : cell->resources) {
				if (!resource.type || !resource.CanUse()) {
					continue;
				}

				// Check if this resource type has a need mapping
				NeedId need_id;
				float restore_amount;
				double use_duration;
				if (mapping.GetNeedForResource(resource.type->name, need_id, restore_amount, use_duration)) {
					// Create a provider for this resource using coordinates
					auto provider = std::make_unique<MapResourceProvider>(
						x, y, resource.type->name, need_id, restore_amount, use_duration
					);
					provider_registry.RegisterProvider(std::move(provider));
				}
			}
		}
	}
}

void ResourceProviderRegistry::Clear() {
	// Note: We don't actually track which providers we created,
	// so we can't selectively clear them. The ProviderRegistry will
	// handle cleanup when providers are destroyed.
	// For a full clear, we'd need ProviderRegistry::Clear(), but that
	// would clear ALL providers, not just resource ones.
}

