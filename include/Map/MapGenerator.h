#ifndef __MAP_GENERATOR__
#define __MAP_GENERATOR__

#include "Map.h"
#include <vector>

/// MapGenerator handles terrain generation logic.
/// It generates walkable/unwalkable tiles and assigns costs to them.
class MapGenerator {
public:
	/// Generates a tile map with the specified dimensions and blocked ratio
	/// @param map The map to populate with generated terrain
	/// @param cols Number of columns
	/// @param rows Number of rows
	/// @param blocked_ratio Ratio of cells that should be blocked (0.0 to 1.0)
	/// @return true if generation was successful
	bool GenerateTileMap(Map& map, int cols, int rows, float blocked_ratio = 0.3f);
	
	/// Loads a map (currently just calls GenerateTileMap with defaults)
	/// @param map The map to populate
	/// @param cost_img Unused (kept for compatibility)
	/// @param terrain_img Unused (kept for compatibility)
	/// @return true if loading was successful
	bool Load(Map& map, const char *cost_img, const char *terrain_img);
	
	/// Initializes a simple grid for headless tests
	/// @param map The map to initialize
	/// @param width Map width
	/// @param height Map height
	/// @param walkable Whether all tiles should be walkable
	void InitializeSynthetic(Map& map, int width, int height, bool walkable = true);
};

#endif // __MAP_GENERATOR__

