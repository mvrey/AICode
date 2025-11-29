//------------------------------------------------------------------------------
// File: ProviderRegistry.cc
// Purpose: Implementation of ProviderRegistry
//------------------------------------------------------------------------------
#include "../../include/Providers/ProviderRegistry.h"
#include <algorithm>
#include <cmath>
#include <limits>

ProviderRegistry& ProviderRegistry::Get() {
	static ProviderRegistry instance;
	return instance;
}

void ProviderRegistry::RegisterProvider(std::unique_ptr<INeedProvider> provider) {
	if (provider) {
		providers_.push_back(std::move(provider));
	}
}

void ProviderRegistry::UnregisterProvider(INeedProvider* provider) {
	providers_.erase(
		std::remove_if(providers_.begin(), providers_.end(),
			[provider](const std::unique_ptr<INeedProvider>& p) {
				return p.get() == provider;
			}),
		providers_.end()
	);
}

std::vector<INeedProvider*> ProviderRegistry::GetProviders(NeedId need_id) const {
	std::vector<INeedProvider*> result;
	for (const auto& provider : providers_) {
		if (provider && provider->GetNeedId() == need_id && provider->IsAvailable()) {
			result.push_back(provider.get());
		}
	}
	return result;
}

INeedProvider* ProviderRegistry::FindNearestProvider(NeedId need_id, const ::MOMOS::Vec2& position) const {
	INeedProvider* nearest = nullptr;
	float nearest_distance = std::numeric_limits<float>::max();

	for (const auto& provider : providers_) {
		if (provider && provider->GetNeedId() == need_id && provider->IsAvailable()) {
			float distance = CalculateDistance(position, provider->GetPosition());
			if (distance < nearest_distance) {
				nearest_distance = distance;
				nearest = provider.get();
			}
		}
	}

	return nearest;
}

bool ProviderRegistry::IsProviderValid(INeedProvider* provider) const {
	if (!provider) {
		return false;
	}
	
	// Check if the provider is still in the registry
	for (const auto& p : providers_) {
		if (p.get() == provider) {
			return true;
		}
	}
	return false;
}

void ProviderRegistry::Clear() {
	providers_.clear();
}

float ProviderRegistry::CalculateDistance(const ::MOMOS::Vec2& a, const ::MOMOS::Vec2& b) const {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return std::sqrt(dx * dx + dy * dy);
}

