//------------------------------------------------------------------------------
// File: ResourceNeedMapping.cc
// Purpose: Implementation of ResourceNeedMapping
//------------------------------------------------------------------------------
#include "../../include/Providers/ResourceNeedMapping.h"

ResourceNeedMapping::ResourceNeedMapping() {
	// Register default mappings
	// Plants restore hunger
	RegisterMapping("TomatoPlant", NeedId::Hunger, 0.3f, 2.0);
	RegisterMapping("StrawberryPlant", NeedId::Hunger, 0.25f, 2.0);
	// Trees could restore hunger if they produce fruit, but for now they produce wood
	// Add more mappings as needed
}

ResourceNeedMapping& ResourceNeedMapping::Get() {
	static ResourceNeedMapping instance;
	return instance;
}

void ResourceNeedMapping::RegisterMapping(const std::string& resource_name, NeedId need_id,
	float restore_amount, double use_duration)
{
	Mapping mapping;
	mapping.need_id = need_id;
	mapping.restore_amount = restore_amount;
	mapping.use_duration = use_duration;
	mappings_[resource_name] = mapping;
}

bool ResourceNeedMapping::GetNeedForResource(const std::string& resource_name, NeedId& out_need_id,
	float& out_restore_amount, double& out_use_duration) const
{
	auto it = mappings_.find(resource_name);
	if (it != mappings_.end()) {
		out_need_id = it->second.need_id;
		out_restore_amount = it->second.restore_amount;
		out_use_duration = it->second.use_duration;
		return true;
	}
	return false;
}

