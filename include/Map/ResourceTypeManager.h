#ifndef RESOURCE_TYPE_MANAGER_H
#define RESOURCE_TYPE_MANAGER_H

#include "MapResourceType.h"
#include <string>
#include <map>
#include <vector>

/// Manages all MapResourceType definitions loaded from JSON.
/// Provides access to resource types by name.
class ResourceTypeManager {
public:
	/// Get the singleton instance
	static ResourceTypeManager& Get();
	
	/// Load resource types from a JSON file
	/// @param json_path Path to the JSON file containing resource type definitions
	/// @return true if loading was successful, false otherwise
	bool LoadFromJSON(const char* json_path);
	
	/// Get a resource type by name
	/// @param name The name of the resource type
	/// @return Pointer to the MapResourceType, or nullptr if not found
	const MapResourceType* GetResourceType(const std::string& name) const;
	
	/// Get all loaded resource types
	/// @return Map of resource type names to their definitions
	const std::map<std::string, MapResourceType>& GetAllResourceTypes() const { return resource_types_; }
	
	/// Check if a resource type exists
	/// @param name The name of the resource type
	/// @return true if the resource type exists, false otherwise
	bool HasResourceType(const std::string& name) const;
	
	/// Clear all loaded resource types
	void Clear();

private:
	ResourceTypeManager() = default;
	~ResourceTypeManager() = default;
	ResourceTypeManager(const ResourceTypeManager&) = delete;
	ResourceTypeManager& operator=(const ResourceTypeManager&) = delete;
	
	// Storage for all resource types, keyed by name
	std::map<std::string, MapResourceType> resource_types_;
	
	// Helper function to parse the JSON file
	bool ParseJSON(const char* json_content);
	
	// Helper function to parse a resource type object
	bool ParseResourceType(const char*& ptr, const std::string& type_name);
};

#endif // RESOURCE_TYPE_MANAGER_H

