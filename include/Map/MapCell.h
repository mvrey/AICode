#ifndef MAP_CELL_H
#define MAP_CELL_H

#include "../config.h"
#include "MapResource.h"
#include <vector>

typedef float Cost;

/// Represents a single cell in the map grid.
/// Contains pathfinding data, cost information, and resources.
class MapCell {
public:
	MapCell();
	~MapCell();

	/// Position of the cell in map coordinates
	::MOMOS::Vec2 position_ = {0, 0};
	
	/// Cost value for pathfinding (0.0 = walkable, 1.0 = blocked)
	Cost cost_ = 0.0f;
	
	/// Pathfinding values (f, g, h for A* algorithm)
	int f = 0;
	int g = 0;
	int h = 0;
	
	/// Parent cell in pathfinding path
	MapCell* parent = nullptr;

	/// List of resources present on this cell
	std::vector<MapResource> resources;

	/// Checks if this cell is walkable (cost < 1.0)
	bool isWalkable() const { return cost_ < 1.0f; }
};

#endif // MAP_CELL_H

