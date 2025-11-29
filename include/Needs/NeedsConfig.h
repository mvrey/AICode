//------------------------------------------------------------------------------
// File: NeedsConfig.h
// Purpose: Configuration loader for need thresholds and default decay rates
//------------------------------------------------------------------------------
#ifndef NEEDS_CONFIG_H
#define NEEDS_CONFIG_H

#include "NeedId.h"
#include <string>
#include <map>

/// Configuration data for a single need type
struct NeedConfig {
	float threshold = 0.3f;
	float default_decay_rate = 0.001f;
};

/// Loads and provides access to need configuration from JSON
class NeedsConfig {
public:
	static NeedsConfig& Get();

	/// Load configuration from JSON file
	/// @param json_path Path to the JSON file
	/// @return true if loading was successful, false otherwise
	bool LoadFromJSON(const char* json_path);

	/// Get configuration for a specific need
	/// @param need_id The need identifier
	/// @return Configuration for the need, or default if not found
	NeedConfig GetConfig(NeedId need_id) const;

private:
	NeedsConfig() = default;
	~NeedsConfig() = default;
	NeedsConfig(const NeedsConfig&) = delete;
	NeedsConfig& operator=(const NeedsConfig&) = delete;

	std::map<NeedId, NeedConfig> configs_;

	/// Parse JSON content
	bool ParseJSON(const char* json_content);
};

#endif // NEEDS_CONFIG_H

