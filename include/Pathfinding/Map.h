#ifndef __MAP__
#define __MAP__

#include "../../include/config.h"
#include <vector>

typedef float Cost;

// Use one of the next declarations
typedef struct cell_s {
	::MOMOS::Vec2 position_ = {0,0};
	Cost cost_ = 0.0f;
	int f, g, h = 0;
	cell_s *parent = nullptr;

	bool isWalkable() const { return cost_ < 1.0f; }
} Cell;

/// Map class holds the core map data and provides access methods.
/// It manages the cell grid and provides coordinate transformations.
class Map {
	int width_;
	int height_;
	std::vector<std::vector<Cell*>> cost_map_;
	
	// Tile data (set by MapGenerator, used by reset())
	std::vector<std::vector<bool>> tile_walkable_;
	std::vector<std::vector<float>> tile_costs_;

public:
	Map();
	~Map();
	
	/// Initializes map dimensions and tile data
	/// Called by MapGenerator to set the terrain data
	void Initialize(int width, int height, 
		const std::vector<std::vector<bool>>& walkable,
		const std::vector<std::vector<float>>& costs);
	
	/// Rebuilds the cost_map_ from tile data
	void reset();
	
	// Getters
	int getHeight() const;
	int getWidth() const;
	Cell* getCellAt(int x, int y);
	const Cell* getCellAt(int x, int y) const;
	
	/// Returns if the given coordinate is walkable in Map coordinates
	bool isWalkable(::MOMOS::Vec2 position) const;
	
	/// Translates window coordinates to internal map coordinates
	::MOMOS::Vec2 ScreenToMapCoords(MOMOS::Vec2 pos) const;
	
	/// Translates internal map coordinates into window coordinates
	::MOMOS::Vec2 MapToScreenCoords(::MOMOS::Vec2 pos) const;
	
	/// Returns if the given coordinate is walkable in Screen coordinates
	bool isWalkableInScreenCoords(::MOMOS::Vec2 pos) const;
	
	/// Prints the cells of cost_map
	void Print() const;
	
	/// Gets tile walkable data (for MapRenderer)
	const std::vector<std::vector<bool>>& GetTileWalkable() const { return tile_walkable_; }
	
	/// Gets tile costs data (for MapRenderer)
	const std::vector<std::vector<float>>& GetTileCosts() const { return tile_costs_; }
};

#endif // __MAP__

