#ifndef __COST_MAP__
#define __COST_MAP__

#include "Map.h"
#include "MapGenerator.h"
#include "MapRenderer.h"

// Forward declaration - Cell is defined in Map.h
typedef struct cell_s Cell;

/// CostMap is a facade that combines Map, MapGenerator, and MapRenderer.
/// It maintains backward compatibility with existing code.
class CostMap
{
	Map map_;
	MapGenerator generator_;
	MapRenderer renderer_;

public:
	CostMap();
	CostMap(const CostMap& orig);
	~CostMap();

	/// Cost map can be a text file or an image file
	bool Load(const char *cost_img, const char *terrain_img);
	bool GenerateTileMap(int cols, int rows, float blocked_ratio = 0.3f);

	/// Initializes a simple grid for headless tests without loading textures.
	void InitializeSynthetic(int width, int height, bool walkable = true);

	void reset();

	// Todo: Add setters / getters
	int getHeight();
	int getWidth();
	Cell* getCellAt(int x, int y);

	/// Returns if the given coordinate is walkable in CostMap coordinates
	bool isWalkable(::MOMOS::Vec2 position);

	/// Translates window coordinates to internal map coordinates
	::MOMOS::Vec2 ScreenToMapCoords(MOMOS::Vec2 pos);

	/// Translates internal map coordinates into window coordinates
	::MOMOS::Vec2 MapToScreenCoords(::MOMOS::Vec2 pos);

	/// Returns if the given coordinate is walkable in Screen coordinates
	bool isWalkableInScreenCoords(::MOMOS::Vec2 pos);

	/// Prints the cells of cost_map
	void Print();

	/// Draws the cost map's tiles if available
	void Draw();
};

#endif // __COST_MAP__
