#include "../../include/Map/MapRenderer.h"
#include "../../include/Map/Map.h"
#include "../../include/Camera.h"
#include "../../include/Map/ResourceTypeManager.h"
#include <MOMOS/draw.h>
#include <MOMOS/sprite.h>

#include <algorithm>
#include <cmath>
#include <string>

namespace {

::MOMOS::SpriteHandle CreatePixelSprite(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	unsigned char pixel[4] = { r, g, b, a };
	return MOMOS::SpriteFromMemory(1, 1, pixel);
}

} // namespace

MapRenderer::MapRenderer() {
	tile_sprite_ = CreatePixelSprite(255, 255, 255, 255); // White sprite (kept for compatibility)
	blocked_tile_sprite_ = CreatePixelSprite(0, 0, 0, 255); // Black sprite (kept for compatibility)
	LoadSprites();
}

MapRenderer::~MapRenderer() {
	ReleaseSprites();
	if (tile_sprite_) {
		MOMOS::SpriteRelease(tile_sprite_);
		tile_sprite_ = nullptr;
	}
	if (blocked_tile_sprite_) {
		MOMOS::SpriteRelease(blocked_tile_sprite_);
		blocked_tile_sprite_ = nullptr;
	}
}

void MapRenderer::LoadSprites() {
	// Load spritesheet and collect sprites
	if (spritesheet_loader_.Load("data/textures.json", "data/textures.png")) {
		// Collect all grass sprites (Grass1.png through Grass9.png)
		for (int i = 1; i <= 9; ++i) {
			std::string grass_name = "Grass" + std::to_string(i) + ".png";
			MOMOS::SpriteHandle grass_sprite = spritesheet_loader_.GetSprite(grass_name);
			if (grass_sprite) {
				grass_sprites_.push_back(grass_sprite);
			}
		}
		
		// Collect medium stone sprites (for cost_ = 1.0)
		for (int i = 1; i <= 2; ++i) {
			std::string stone_name = "MediumStone" + std::to_string(i) + ".png";
			MOMOS::SpriteHandle stone_sprite = spritesheet_loader_.GetSprite(stone_name);
			if (stone_sprite) {
				medium_stone_sprites_.push_back(stone_sprite);
			}
		}
		
		// Collect small stone sprites (for cost_ = 0.75)
		for (int i = 1; i <= 5; ++i) {
			std::string stone_name = "SmallStone" + std::to_string(i) + ".png";
			MOMOS::SpriteHandle stone_sprite = spritesheet_loader_.GetSprite(stone_name);
			if (stone_sprite) {
				small_stone_sprites_.push_back(stone_sprite);
			}
		}
		
		// Collect dirt sprites (for cost_ = 0.5)
		for (int i = 1; i <= 2; ++i) {
			std::string dirt_name = "Dirt" + std::to_string(i) + ".png";
			MOMOS::SpriteHandle dirt_sprite = spritesheet_loader_.GetSprite(dirt_name);
			if (dirt_sprite) {
				dirt_sprites_.push_back(dirt_sprite);
			}
		}
	}
}

void MapRenderer::ReleaseSprites() {
	// Release grass sprites
	for (MOMOS::SpriteHandle sprite : grass_sprites_) {
		if (sprite) {
			MOMOS::SpriteRelease(sprite);
		}
	}
	grass_sprites_.clear();
	
	// Release medium stone sprites
	for (MOMOS::SpriteHandle sprite : medium_stone_sprites_) {
		if (sprite) {
			MOMOS::SpriteRelease(sprite);
		}
	}
	medium_stone_sprites_.clear();
	
	// Release small stone sprites
	for (MOMOS::SpriteHandle sprite : small_stone_sprites_) {
		if (sprite) {
			MOMOS::SpriteRelease(sprite);
		}
	}
	small_stone_sprites_.clear();
	
	// Release dirt sprites
	for (MOMOS::SpriteHandle sprite : dirt_sprites_) {
		if (sprite) {
			MOMOS::SpriteRelease(sprite);
		}
	}
	dirt_sprites_.clear();
	
	// Clear the loader's sprite map to prevent it from trying to release sprites we've already released
	spritesheet_loader_.ClearSpriteMap();
}

void MapRenderer::Draw(const Map& map) {
	int width = map.getWidth();
	int height = map.getHeight();
	
	if (width <= 0 || height <= 0) {
		return;
	}

	float tile_world_width = static_cast<float>(Screen::width) / static_cast<float>(width);
	float tile_world_height = static_cast<float>(Screen::height) / static_cast<float>(height);

	::MOMOS::Vec2 view_world_top_left = Camera::ScreenToWorld({ 0.0f, 0.0f });
	::MOMOS::Vec2 view_world_bottom_right = Camera::ScreenToWorld({ static_cast<float>(Screen::width), static_cast<float>(Screen::height) });

	const float view_min_x = std::min(view_world_top_left.x, view_world_bottom_right.x);
	const float view_max_x = std::max(view_world_top_left.x, view_world_bottom_right.x);
	const float view_min_y = std::min(view_world_top_left.y, view_world_bottom_right.y);
	const float view_max_y = std::max(view_world_top_left.y, view_world_bottom_right.y);

	int start_x = static_cast<int>(std::floor(view_min_x / tile_world_width));
	int end_x = static_cast<int>(std::ceil(view_max_x / tile_world_width)) - 1;
	int start_y = static_cast<int>(std::floor(view_min_y / tile_world_height));
	int end_y = static_cast<int>(std::ceil(view_max_y / tile_world_height)) - 1;

	start_x = std::max(0, start_x);
	start_y = std::max(0, start_y);
	end_x = std::min(width - 1, end_x);
	end_y = std::min(height - 1, end_y);

	if (start_x <= end_x && start_y <= end_y) {
		for (int x = start_x; x <= end_x; ++x) {
			for (int y = start_y; y <= end_y; ++y) {
				::MOMOS::Vec2 world_top_left = { x * tile_world_width, y * tile_world_height };
				::MOMOS::Vec2 world_bottom_right = { (x + 1) * tile_world_width, (y + 1) * tile_world_height };
				::MOMOS::Vec2 screen_top_left = Camera::WorldToScreen(world_top_left);
				::MOMOS::Vec2 screen_bottom_right = Camera::WorldToScreen(world_bottom_right);

				// Get the cell and its cost
				const MapCell* cell = map.getCellAt(x, y);
				float cost = 0.0f;
				if (cell != nullptr) {
					cost = cell->cost_;
				}
				
				// Draw grass sprite on 70% of cells with cost_ == 0
				if (cost == 0.0f && !grass_sprites_.empty()) {
					// Use deterministic hash based on cell position for consistent randomness
					unsigned int hash = static_cast<unsigned int>(x * 73856093u) ^ static_cast<unsigned int>(y * 19349663u);
					
					// 70% chance to draw grass (using hash % 100 < 70)
					if ((hash % 100) < 70) {
						// Select random grass sprite based on cell position
						unsigned int sprite_hash = static_cast<unsigned int>(x * 19349669u) ^ static_cast<unsigned int>(y * 83492791u);
						unsigned int sprite_index = sprite_hash % static_cast<unsigned int>(grass_sprites_.size());
						MOMOS::SpriteHandle grass_sprite = grass_sprites_[sprite_index];
						
						if (grass_sprite) {
							// Calculate center of the cell in screen coordinates
							::MOMOS::Vec2 world_center = {
								(x + 0.5f) * tile_world_width,
								(y + 0.5f) * tile_world_height
							};
							::MOMOS::Vec2 screen_center = Camera::WorldToScreen(world_center);
							
							// Get sprite dimensions and apply camera zoom
							float zoom = Camera::Zoom();
							int sprite_width = MOMOS::SpriteWidth(grass_sprite);
							int sprite_height = MOMOS::SpriteHeight(grass_sprite);
							float half_width = sprite_width * 0.5f * zoom;
							float half_height = sprite_height * 0.5f * zoom;
							
							// Draw sprite with zoom scaling
							::MOMOS::SpriteTransform sprite_transform{};
							sprite_transform.x = screen_center.x - half_width;
							sprite_transform.y = screen_center.y - half_height;
							sprite_transform.scale_x = zoom;
							sprite_transform.scale_y = zoom;
							MOMOS::DrawSprite(grass_sprite, sprite_transform);
						}
					}
				}
				
				// Draw sprites based on cost value
				MOMOS::SpriteHandle cost_sprite = nullptr;
				const float epsilon = 0.01f;
				
				// cost_ = 1.0: MediumStone1 or MediumStone2
				if (std::abs(cost - 1.0f) < epsilon && !medium_stone_sprites_.empty()) {
					unsigned int sprite_hash = static_cast<unsigned int>(x * 19349669u) ^ static_cast<unsigned int>(y * 83492791u);
					unsigned int sprite_index = sprite_hash % static_cast<unsigned int>(medium_stone_sprites_.size());
					cost_sprite = medium_stone_sprites_[sprite_index];
				}
				// cost_ = 0.75: SmallStone1-5
				else if (std::abs(cost - 0.75f) < epsilon && !small_stone_sprites_.empty()) {
					unsigned int sprite_hash = static_cast<unsigned int>(x * 19349669u) ^ static_cast<unsigned int>(y * 83492791u);
					unsigned int sprite_index = sprite_hash % static_cast<unsigned int>(small_stone_sprites_.size());
					cost_sprite = small_stone_sprites_[sprite_index];
				}
				// cost_ = 0.5: Dirt1 or Dirt2
				else if (std::abs(cost - 0.5f) < epsilon && !dirt_sprites_.empty()) {
					unsigned int sprite_hash = static_cast<unsigned int>(x * 19349669u) ^ static_cast<unsigned int>(y * 83492791u);
					unsigned int sprite_index = sprite_hash % static_cast<unsigned int>(dirt_sprites_.size());
					cost_sprite = dirt_sprites_[sprite_index];
				}
				
				// Draw the cost sprite if one was selected
				if (cost_sprite) {
					// Calculate center of the cell in screen coordinates
					::MOMOS::Vec2 world_center = {
						(x + 0.5f) * tile_world_width,
						(y + 0.5f) * tile_world_height
					};
					::MOMOS::Vec2 screen_center = Camera::WorldToScreen(world_center);
					
					// Get sprite dimensions and apply camera zoom
					float zoom = Camera::Zoom();
					int sprite_width = MOMOS::SpriteWidth(cost_sprite);
					int sprite_height = MOMOS::SpriteHeight(cost_sprite);
					float half_width = sprite_width * 0.5f * zoom;
					float half_height = sprite_height * 0.5f * zoom;
					
					// Draw sprite with zoom scaling
					::MOMOS::SpriteTransform sprite_transform{};
					sprite_transform.x = screen_center.x - half_width;
					sprite_transform.y = screen_center.y - half_height;
					sprite_transform.scale_x = zoom;
					sprite_transform.scale_y = zoom;
					MOMOS::DrawSprite(cost_sprite, sprite_transform);
				}
			}
		}
	}
	
	// Second pass: Draw resources from cells on top of all other sprites
	if (start_x <= end_x && start_y <= end_y) {
		for (int x = start_x; x <= end_x; ++x) {
			for (int y = start_y; y <= end_y; ++y) {
				const MapCell* cell = map.getCellAt(x, y);
				if (cell == nullptr || cell->resources.empty()) {
					continue;
				}
				
				// Draw each resource in the cell
				for (const auto& resource : cell->resources) {
					if (resource.type == nullptr || resource.amount <= 0) {
						continue;
					}
					
					// Get the resource type's image names
					const std::vector<std::string>& image_names = resource.type->image_names;
					if (image_names.empty()) {
						continue;
					}
					
					// Select an image based on cell position for consistency
					unsigned int image_hash = static_cast<unsigned int>(x * 19349669u) ^ static_cast<unsigned int>(y * 83492791u);
					unsigned int image_index = image_hash % static_cast<unsigned int>(image_names.size());
					const std::string& image_name = image_names[image_index];
					
					// Get sprite from spritesheet loader
					MOMOS::SpriteHandle resource_sprite = spritesheet_loader_.GetSprite(image_name);
					if (resource_sprite) {
						// Calculate center of the cell in screen coordinates
						::MOMOS::Vec2 world_center = {
							(x + 0.5f) * tile_world_width,
							(y + 0.5f) * tile_world_height
						};
						::MOMOS::Vec2 screen_center = Camera::WorldToScreen(world_center);
						
						// Get sprite dimensions and apply camera zoom
						float zoom = Camera::Zoom();
						int sprite_width = MOMOS::SpriteWidth(resource_sprite);
						int sprite_height = MOMOS::SpriteHeight(resource_sprite);
						float half_width = sprite_width * 0.5f * zoom;
						float half_height = sprite_height * 0.5f * zoom;
						
						// Draw sprite with zoom scaling
						::MOMOS::SpriteTransform sprite_transform{};
						sprite_transform.x = screen_center.x - half_width;
						sprite_transform.y = screen_center.y - half_height;
						sprite_transform.scale_x = zoom;
						sprite_transform.scale_y = zoom;
						MOMOS::DrawSprite(resource_sprite, sprite_transform);
					}
				}
			}
		}
	}
}

