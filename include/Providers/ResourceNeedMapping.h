//------------------------------------------------------------------------------
// File: ResourceNeedMapping.h
// Purpose: Maps resource types to needs they satisfy
//------------------------------------------------------------------------------
#ifndef RESOURCE_NEED_MAPPING_H
#define RESOURCE_NEED_MAPPING_H

#include "../Needs/NeedId.h"
#include "../Map/MapResourceType.h"
#include <map>

/// Maps resource type names to the needs they satisfy
/// This allows resources to automatically become need providers
class ResourceNeedMapping {
public:
	static ResourceNeedMapping& Get();

	/// Register a mapping from resource type name to need
	/// @param resource_name Name of the resource type (e.g., "Tree", "TomatoPlant")
	/// @param need_id The need this resource satisfies
	/// @param restore_amount How much this resource restores (0.0 to 1.0)
	/// @param use_duration How long it takes to use this resource (seconds)
	void RegisterMapping(const std::string& resource_name, NeedId need_id, 
		float restore_amount, double use_duration);

	/// Get the need ID for a resource type
	/// @param resource_name Name of the resource type
	/// @return The need ID, or nullptr if not mapped
	bool GetNeedForResource(const std::string& resource_name, NeedId& out_need_id, 
		float& out_restore_amount, double& out_use_duration) const;

private:
	ResourceNeedMapping();
	~ResourceNeedMapping() = default;
	ResourceNeedMapping(const ResourceNeedMapping&) = delete;
	ResourceNeedMapping& operator=(const ResourceNeedMapping&) = delete;

	struct Mapping {
		NeedId need_id;
		float restore_amount;
		double use_duration;
	};

	std::map<std::string, Mapping> mappings_;
};

#endif // RESOURCE_NEED_MAPPING_H

