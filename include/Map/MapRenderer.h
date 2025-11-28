#ifndef __MAP_RENDERER__
#define __MAP_RENDERER__

#include "Map.h"
#include "../SpritesheetLoader.h"
#include <vector>

/// MapRenderer handles all drawing operations for the map.
/// It manages sprites and renders them based on map data.
class MapRenderer {
	SpritesheetLoader spritesheet_loader_;
	
	// Sprite collections
	std::vector<MOMOS::SpriteHandle> grass_sprites_;
	std::vector<MOMOS::SpriteHandle> medium_stone_sprites_;
	std::vector<MOMOS::SpriteHandle> small_stone_sprites_;
	std::vector<MOMOS::SpriteHandle> dirt_sprites_;
	std::vector<MOMOS::SpriteHandle> tree_sprites_;
	
	// Legacy sprites (kept for compatibility)
	::MOMOS::SpriteHandle tile_sprite_;
	::MOMOS::SpriteHandle blocked_tile_sprite_;

public:
	MapRenderer();
	~MapRenderer();
	
	/// Draws the map using the provided Map data
	/// @param map The map to draw
	void Draw(const Map& map);
	
private:
	/// Loads all sprites from the spritesheet
	void LoadSprites();
	
	/// Releases all loaded sprites
	void ReleaseSprites();
};

#endif // __MAP_RENDERER__

