//------------------------------------------------------------------------------
// File: ResourceProviderRegistry.cc
// Purpose: Implementation of ResourceProviderRegistry
//------------------------------------------------------------------------------
#include "../../include/Providers/ResourceProviderRegistry.h"
#include "../../include/Providers/ProviderRegistry.h"
#include "../../include/Providers/MapResourceProvider.h"
#include "../../include/Providers/ResourceNeedMapping.h"
#include "../../include/Providers/IMapResourceQuery.h"
#include "../../include/Map/MapCell.h"
#include "../../include/Map/MapResource.h"
#include <memory>

ResourceProviderRegistry& ResourceProviderRegistry::Get() {
	static ResourceProviderRegistry instance;
	return instance;
}

void ResourceProviderRegistry::RegisterMapResources(IMapResourceQuery& map_query) {
	// Clear existing resource providers first
	Clear();

	ResourceNeedMapping& mapping = ResourceNeedMapping::Get();
	ProviderRegistry& provider_registry = ProviderRegistry::Get();

	// Get dimensions from interface
	int width = map_query.GetWidth();
	int height = map_query.GetHeight();

	// Scan all cells and register resources as providers
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			// Get resources using the interface
			std::vector<MapResource> resources = map_query.GetResourcesAt(x, y);
			
			// Check each resource
			for (auto& resource : resources) {
				if (!resource.type || !resource.CanUse()) {
					continue;
				}

				// Check if this resource type has a need mapping
				NeedId need_id;
				float restore_amount;
				double use_duration;
				if (mapping.GetNeedForResource(resource.type->name, need_id, restore_amount, use_duration)) {
					// Create a provider for this resource
					// MapResourceProvider will get the Map from MapService/GameStatus when needed
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

