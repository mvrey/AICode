#ifndef MAP_RESOURCE_TYPE_H
#define MAP_RESOURCE_TYPE_H

#include <string>
#include <vector>
#include <map>
#include <utility>

// Forward declaration
class MapResource;

/// Represents a type of resource that can exist on the map.
/// Defines properties like what it produces, how it's rendered, etc.
class MapResourceType {
public:
	MapResourceType();
	MapResourceType(const std::string& name);
	~MapResourceType();

	/// Name of the resource type (e.g., "Tree", "Stone", "Berry Bush")
	std::string name;

	/// List of image names (references in the texture mapping/spritesheet)
	/// Used for rendering different variants of this resource type
	std::vector<std::string> image_names;

	/// If true, this resource is destroyed/consumed when used
	bool destroy_on_use = false;

	/// Maximum stack size for this resource type
	int max_stack = 1;

	/// Dictionary mapping output resources to their quantity range [min, max]
	/// When this resource is used, it produces these outputs
	/// Key: pointer to MapResourceType that is produced
	/// Value: pair of (min_amount, max_amount)
	std::map<const MapResourceType*, std::pair<int, int>> resource_outputs;
};

#endif // MAP_RESOURCE_TYPE_H

