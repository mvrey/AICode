//------------------------------------------------------------------------------
// File: ResourceProviderRegistry.h
// Purpose: Auto-registers MapResources as need providers
//------------------------------------------------------------------------------
#ifndef RESOURCE_PROVIDER_REGISTRY_H
#define RESOURCE_PROVIDER_REGISTRY_H

#include "../Pathfinding/cost_map.h"

/// System that automatically registers MapResources as need providers
/// Scans the map and creates providers for resources that have need mappings
class ResourceProviderRegistry {
public:
	static ResourceProviderRegistry& Get();

	/// Scan the map and register all resources as providers
	/// Should be called after map generation and whenever map changes
	void RegisterMapResources(CostMap& cost_map);

	/// Clear all registered resource providers
	void Clear();

private:
	ResourceProviderRegistry() = default;
	~ResourceProviderRegistry() = default;
	ResourceProviderRegistry(const ResourceProviderRegistry&) = delete;
	ResourceProviderRegistry& operator=(const ResourceProviderRegistry&) = delete;
};

#endif // RESOURCE_PROVIDER_REGISTRY_H

