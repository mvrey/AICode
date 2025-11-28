#include "../../include/Pathfinding/cost_map.h"
#include "../../include/Camera.h"
#include <MOMOS/draw.h>
#include <MOMOS/sprite.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>
#include <vector>
#include <string>

namespace {

::MOMOS::SpriteHandle CreatePixelSprite(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	unsigned char pixel[4] = { r, g, b, a };
	return MOMOS::SpriteFromMemory(1, 1, pixel);
}

} // namespace

CostMap::CostMap() {
	width_ = 0;
	height_ = 0;
	tile_sprite_ = CreatePixelSprite(255, 255, 255, 255); // White sprite (kept for compatibility)
	blocked_tile_sprite_ = CreatePixelSprite(0, 0, 0, 255); // Black sprite (kept for compatibility)
	
	// Load spritesheet and collect grass sprites
	if (spritesheet_loader_.Load("data/textures.json", "data/textures.png")) {
		// Collect all grass sprites (Grass1.png through Grass9.png)
		for (int i = 1; i <= 9; ++i) {
			std::string grass_name = "Grass" + std::to_string(i) + ".png";
			MOMOS::SpriteHandle grass_sprite = spritesheet_loader_.GetSprite(grass_name);
			if (grass_sprite) {
				grass_sprites_.push_back(grass_sprite);
			}
		}
	}
}

CostMap::CostMap(const CostMap& orig) {}
CostMap::~CostMap() {
	reset();
	if (tile_sprite_) {
		MOMOS::SpriteRelease(tile_sprite_);
		tile_sprite_ = nullptr;
	}
	if (blocked_tile_sprite_) {
		MOMOS::SpriteRelease(blocked_tile_sprite_);
		blocked_tile_sprite_ = nullptr;
	}
	
	// Release grass sprites (they are independent textures created by SubSprite)
	for (MOMOS::SpriteHandle sprite : grass_sprites_) {
		if (sprite) {
			MOMOS::SpriteRelease(sprite);
		}
	}
	grass_sprites_.clear();
	
	// Clear the loader's sprite map to prevent it from trying to release sprites we've already released
	spritesheet_loader_.ClearSpriteMap();
}


bool CostMap::Load(const char *cost_img, const char *terrain_img) {
	static constexpr int kDefaultCols = 80;
	static constexpr int kDefaultRows = 45;
	static constexpr float kDefaultBlockedRatio = 0.3f;

	return GenerateTileMap(kDefaultCols, kDefaultRows, kDefaultBlockedRatio);
}

bool CostMap::GenerateTileMap(int cols, int rows, float blocked_ratio) {
	const int safe_cols = cols > 0 ? cols : 1;
	const int safe_rows = rows > 0 ? rows : 1;
	width_ = safe_cols;
	height_ = safe_rows;
	tile_walkable_.assign(width_, std::vector<bool>(height_, true));
	tile_costs_.assign(width_, std::vector<float>(height_, 0.0f)); // Initialize all costs to 0.0f

	float clamped_ratio = blocked_ratio;
	if (clamped_ratio < 0.0f) {
		clamped_ratio = 0.0f;
	} else if (clamped_ratio > 1.0f) {
		clamped_ratio = 1.0f;
	}

	const int total_cells = width_ * height_;
	const int desired_blocked = static_cast<int>(clamped_ratio * total_cells);

	auto is_reserved_tile = [width = width_, height = height_](int x, int y) {
		if (x == 0 && y == 0) {
			return true;
		}
		if (width > 1 && x == 1 && y == 0) {
			return true;
		}
		if (height > 1 && x == 0 && y == 1) {
			return true;
		}
		return false;
	};

	int blocked_cells = 0;
	constexpr int kMinLumpSize = 2;
	constexpr int kMaxLumpSize = 8;
	const int kMaxPlacementAttempts = 500;
	const int kFailureThresholdForSingleCell = std::max(1, (width_ * height_) / 4);
	const int kMaxTotalFailures = (width_ * height_) * 2;
	int total_failure_count = 0;

	while (blocked_cells < desired_blocked && total_failure_count < kMaxTotalFailures) {
		int remaining = desired_blocked - blocked_cells;
		if (remaining <= 0) {
			break;
		}

		bool allow_single_cell = total_failure_count >= kFailureThresholdForSingleCell;
		int lump_min = (remaining >= kMinLumpSize && !allow_single_cell) ? kMinLumpSize : 1;
		if (lump_min > remaining) {
			lump_min = remaining;
		}

		int lump_max = std::min(kMaxLumpSize, remaining);
		if (lump_max < lump_min) {
			lump_max = lump_min;
		}

		int target_size = lump_min;
		if (lump_max > lump_min) {
			target_size += rand() % (lump_max - lump_min + 1);
		}

		bool placed = false;
		for (int placement_attempt = 0; placement_attempt < kMaxPlacementAttempts && !placed; ++placement_attempt) {
			int start_x = rand() % width_;
			int start_y = rand() % height_;
			if (!tile_walkable_[start_x][start_y] || is_reserved_tile(start_x, start_y)) {
				continue;
			}

			std::vector<std::vector<bool>> visited(width_, std::vector<bool>(height_, false));
			std::vector<std::pair<int, int>> lump_cells;
			lump_cells.emplace_back(start_x, start_y);
			visited[start_x][start_y] = true;
			std::vector<std::pair<int, int>> frontier = lump_cells;

			while (static_cast<int>(lump_cells.size()) < target_size) {
				std::vector<std::pair<int, int>> candidates;
				const int kDx[4] = { 1, -1, 0, 0 };
				const int kDy[4] = { 0, 0, 1, -1 };
				for (const auto& cell : frontier) {
					for (int dir = 0; dir < 4; ++dir) {
						int nx = cell.first + kDx[dir];
						int ny = cell.second + kDy[dir];
						if (nx < 0 || nx >= width_ || ny < 0 || ny >= height_) {
							continue;
						}
						if (is_reserved_tile(nx, ny) || !tile_walkable_[nx][ny] || visited[nx][ny]) {
							continue;
						}
						candidates.emplace_back(nx, ny);
					}
				}

				if (candidates.empty()) {
					break;
				}

				auto next_cell = candidates[rand() % candidates.size()];
				lump_cells.push_back(next_cell);
				visited[next_cell.first][next_cell.second] = true;
				frontier.push_back(next_cell);
			}

			if (static_cast<int>(lump_cells.size()) >= lump_min) {
				for (const auto& cell : lump_cells) {
					int x = cell.first;
					int y = cell.second;
					if (tile_walkable_[x][y]) {
						tile_walkable_[x][y] = false;
						// Assign random cost from {0.5f, 0.75f, 1.0f} for unwalkable cells
						int random_choice = rand() % 3;
						switch (random_choice) {
							case 0:
								tile_costs_[x][y] = 0.5f;
								break;
							case 1:
								tile_costs_[x][y] = 0.75f;
								break;
							case 2:
							default:
								tile_costs_[x][y] = 1.0f;
								break;
						}
						++blocked_cells;
						if (blocked_cells >= desired_blocked) {
							break;
						}
					}
				}
				placed = true;
			}
		}

		if (placed) {
			total_failure_count = 0;
		} else {
			++total_failure_count;
		}
	}

	tile_walkable_[0][0] = true;
	tile_costs_[0][0] = 0.0f;
	if (width_ > 1) {
		tile_walkable_[1][0] = true;
		tile_costs_[1][0] = 0.0f;
	}
	if (height_ > 1) {
		tile_walkable_[0][1] = true;
		tile_costs_[0][1] = 0.0f;
	}

	reset();
	return true;
}

void CostMap::InitializeSynthetic(int width, int height, bool walkable) {
	width_ = (width > 0) ? width : 1;
	height_ = (height > 0) ? height : 1;
	tile_walkable_.assign(width_, std::vector<bool>(height_, walkable));
	reset();
}


void CostMap::reset() {
	for (auto& column : cost_map_) {
		for (Cell* cell : column) {
			delete cell;
		}
		column.clear();
	}
	cost_map_.clear();

	if (width_ <= 0 || height_ <= 0) {
		return;
	}

	for (int w = 0; w < width_; w++) {
		cost_map_.emplace_back();

		for (int h = 0; h < height_; h++) {
			Cell* cell = new Cell();
			cell->position_.x = static_cast<float>(w);
			cell->position_.y = static_cast<float>(h);

			bool walkable = true;
			float cost = 0.0f;
			if (w < static_cast<int>(tile_walkable_.size()) && h < static_cast<int>(tile_walkable_[w].size())) {
				walkable = tile_walkable_[w][h];
				// Use stored cost if available, otherwise default to 0.0f for walkable, 1.0f for unwalkable
				if (w < static_cast<int>(tile_costs_.size()) && h < static_cast<int>(tile_costs_[w].size())) {
					cost = tile_costs_[w][h];
				} else if (!walkable) {
					cost = 1.0f; // Default cost for unwalkable cells if not stored
				}
			}
			
			cell->cost_ = cost;

			cost_map_[w].push_back(cell);
		}
	}
}


int CostMap::getHeight() {
	return height_;
}


int CostMap::getWidth() {
	return width_;
}


Cell* CostMap::getCellAt(int x, int y) {
	if (x >= 0 && y >= 0 && x < width_ && y < height_)
		return cost_map_[x][y];

	return nullptr;
}


bool CostMap::isWalkable(MOMOS::Vec2 position) {
	return getCellAt(static_cast<int>(position.x), static_cast<int>(position.y))->isWalkable();
}


/// Translates window coordinates to internal map coordinates
MOMOS::Vec2 CostMap::ScreenToMapCoords(MOMOS::Vec2 pos) {
	MOMOS::Vec2 map_coords;
	map_coords.x = (pos.x > 0.0f) ? (float)((int)round(pos.x / ((float)Screen::width / (float)width_)) % width_) : 0;
	map_coords.y = (pos.y > 0.0f) ? (float)((int)round(pos.y / ((float)Screen::height / (float)height_)) % height_) : 0;

	return map_coords;
}


MOMOS::Vec2 CostMap::MapToScreenCoords(MOMOS::Vec2 pos) {
	MOMOS::Vec2 scr_coords;
	scr_coords.x = pos.x * ((float)Screen::width / (float)width_);
	scr_coords.y = pos.y * ((float)Screen::height / (float)height_);

	return scr_coords;
}

bool CostMap::isWalkableInScreenCoords(MOMOS::Vec2 pos) {
	return isWalkable(ScreenToMapCoords(pos));
}


void CostMap::Print() {
	for (int i = 0; i < width_; i++) {
		printf("\n");
		for (int j = 0; j < height_; j++) {
			printf("%d  ", cost_map_[i][j]->f);
		}
	}
}


void CostMap::Draw() {
	if (width_ <= 0 || height_ <= 0) {
		return;
	}

	float tile_world_width = static_cast<float>(Screen::width) / static_cast<float>(width_);
	float tile_world_height = static_cast<float>(Screen::height) / static_cast<float>(height_);

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
	end_x = std::min(width_ - 1, end_x);
	end_y = std::min(height_ - 1, end_y);

	if (start_x <= end_x && start_y <= end_y) {
		for (int x = start_x; x <= end_x; ++x) {
			for (int y = start_y; y <= end_y; ++y) {
				::MOMOS::Vec2 world_top_left = { x * tile_world_width, y * tile_world_height };
				::MOMOS::Vec2 world_bottom_right = { (x + 1) * tile_world_width, (y + 1) * tile_world_height };
				::MOMOS::Vec2 screen_top_left = Camera::WorldToScreen(world_top_left);
				::MOMOS::Vec2 screen_bottom_right = Camera::WorldToScreen(world_bottom_right);

				// Get the cell and its cost to determine the color
				Cell* cell = cost_map_[x][y];
				float cost = 0.0f;
				if (cell != nullptr) {
					cost = cell->cost_;
				}
				
				// Calculate color based on cost: cost_ = 0.0 is white (255), cost_ = 1.0 is black (0)
				// Formula: color = 255 * (1.0 - cost)
				unsigned char grey_value = static_cast<unsigned char>(255.0f * (1.0f - cost));
				
				// Draw colored rectangle based on cost
				float points[10] = {
					screen_top_left.x, screen_top_left.y,
					screen_bottom_right.x, screen_top_left.y,
					screen_bottom_right.x, screen_bottom_right.y,
					screen_top_left.x, screen_bottom_right.y,
					screen_top_left.x, screen_top_left.y
				};
				
				MOMOS::DrawSetFillColor(grey_value, grey_value, grey_value, 255);
				MOMOS::DrawSolidPath(points, 5, false);
				
				// Draw grass sprite on 70% of cells with cost_ == 0
				if (cost == 0.0f && !grass_sprites_.empty()) {
					// Use deterministic hash based on cell position for consistent randomness
					// This ensures the same cell always gets the same random selection
					unsigned int hash = static_cast<unsigned int>(x * 73856093u) ^ static_cast<unsigned int>(y * 19349663u);
					
					// 70% chance to draw grass (using hash % 100 < 70)
					if ((hash % 100) < 70) {
						// Select random grass sprite based on cell position (use different hash for sprite selection)
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
							
							// Get sprite dimensions
							int sprite_width = MOMOS::SpriteWidth(grass_sprite);
							int sprite_height = MOMOS::SpriteHeight(grass_sprite);
							
							// Draw sprite centered on the cell
							MOMOS::DrawSprite(grass_sprite, 
								screen_center.x - sprite_width * 0.5f,
								screen_center.y - sprite_height * 0.5f);
						}
					}
				}
			}
		}
	}

	MOMOS::DrawSetStrokeColor(180, 180, 180);
	float total_world_width = static_cast<float>(Screen::width);
	float total_world_height = static_cast<float>(Screen::height);

	for (int col = 0; col <= width_; ++col) {
		float world_x = col * tile_world_width;
		::MOMOS::Vec2 top = Camera::WorldToScreen({ world_x, 0.0f });
		::MOMOS::Vec2 bottom = Camera::WorldToScreen({ world_x, total_world_height });
		MOMOS::DrawLine(top.x, top.y, bottom.x, bottom.y);
	}

	for (int row = 0; row <= height_; ++row) {
		float world_y = row * tile_world_height;
		::MOMOS::Vec2 left = Camera::WorldToScreen({ 0.0f, world_y });
		::MOMOS::Vec2 right = Camera::WorldToScreen({ total_world_width, world_y });
		MOMOS::DrawLine(left.x, left.y, right.x, right.y);
	}
}