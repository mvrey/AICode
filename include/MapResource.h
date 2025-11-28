#ifndef MAP_RESOURCE_H
#define MAP_RESOURCE_H

#include "MapResourceType.h"
#include <vector>

/// Represents an instance of a resource on a map cell.
/// Contains a reference to the resource type and the amount present.
class MapResource {
public:
	MapResource();
	MapResource(const MapResourceType* type, int amount);
	~MapResource();

	/// The type of resource this instance represents
	const MapResourceType* type = nullptr;

	/// The amount of this resource present
	int amount = 0;

	/// Checks if this resource can be used (has valid type and amount > 0)
	bool CanUse() const;

	/// Uses this resource, consuming it if destroy_on_use is true
	/// Returns a vector of resources produced (outputs)
	std::vector<MapResource> Use() const;
};

#endif // MAP_RESOURCE_H

