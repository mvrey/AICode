//------------------------------------------------------------------------------
// File: ProviderRegistry.h
// Purpose: Central registry for all need providers in the world
//------------------------------------------------------------------------------
#ifndef PROVIDER_REGISTRY_H
#define PROVIDER_REGISTRY_H

#include "INeedProvider.h"
#include "../Needs/NeedId.h"
#include <vector>
#include <memory>

/// Central registry that manages all need providers in the world
/// Allows querying providers by need type and finding the nearest one
class ProviderRegistry {
public:
	static ProviderRegistry& Get();

	/// Register a provider (takes ownership)
	void RegisterProvider(std::unique_ptr<INeedProvider> provider);

	/// Unregister a provider (removes it from the registry)
	void UnregisterProvider(INeedProvider* provider);

	/// Get all providers that satisfy a specific need
	/// @param need_id The need to find providers for
	/// @return Vector of available providers for that need
	std::vector<INeedProvider*> GetProviders(NeedId need_id) const;

	/// Find the nearest available provider for a specific need
	/// @param need_id The need to find a provider for
	/// @param position The position to search from
	/// @return The nearest available provider, or nullptr if none found
	INeedProvider* FindNearestProvider(NeedId need_id, const ::MOMOS::Vec2& position) const;

	/// Clear all registered providers
	void Clear();

private:
	ProviderRegistry() = default;
	~ProviderRegistry() = default;
	ProviderRegistry(const ProviderRegistry&) = delete;
	ProviderRegistry& operator=(const ProviderRegistry&) = delete;

	std::vector<std::unique_ptr<INeedProvider>> providers_;

	float CalculateDistance(const ::MOMOS::Vec2& a, const ::MOMOS::Vec2& b) const;
};

#endif // PROVIDER_REGISTRY_H

