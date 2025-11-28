
// project A Star
// cost_map.h
// author: Toni Barella
// Artificial Intelligence - 3VGP
// ESAT 2016/2017

#ifndef __COST_MAP__
#define __COST_MAP__

#include <cmath>
#include <ctgmath>
#include "../../include/config.h"
#include "../../include/SpritesheetLoader.h"
#include <vector>

typedef float Cost;

// Use one of the next declarations
typedef struct cell_s {
	::MOMOS::Vec2 position_ = {0,0};
	Cost cost_ = 0.0f;
	int f, g, h = 0;
	cell_s *parent = nullptr;

	bool isWalkable() { return cost_ < 1.0f; }
} Cell;

class CostMap
{
	int width_;
	int height_;
	std::vector<std::vector<Cell*>> cost_map_;

	::MOMOS::SpriteHandle tile_sprite_;
	::MOMOS::SpriteHandle blocked_tile_sprite_;
	std::vector<std::vector<bool>> tile_walkable_;
	std::vector<std::vector<float>> tile_costs_; // Store cost values to preserve them across resets
	
	SpritesheetLoader spritesheet_loader_;  ///< Loader for grass sprites
	std::vector<MOMOS::SpriteHandle> grass_sprites_;  ///< Available grass sprites

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