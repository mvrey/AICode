#include "../../include/Map/Map.h"
#include "../../include/Camera.h"
#include "../../include/UI/InfoPanel.h"
#include <MOMOS/input.h>
#include <MOMOS/draw.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

Map::Map() : width_(0), height_(0) {
}

Map::~Map() {
	reset(); // This will delete all cells
}

void Map::Initialize(int width, int height,
	const std::vector<std::vector<bool>>& walkable,
	const std::vector<std::vector<float>>& costs) {
	width_ = width;
	height_ = height;
	tile_walkable_ = walkable;
	tile_costs_ = costs;
	reset();
}

void Map::reset() {
	// Delete existing cells
	for (auto& column : cost_map_) {
		for (MapCell* cell : column) {
			delete cell;
		}
		column.clear();
	}
	cost_map_.clear();

	if (width_ <= 0 || height_ <= 0) {
		return;
	}

	// Rebuild cost_map_ from tile data
	for (int w = 0; w < width_; w++) {
		cost_map_.emplace_back();

		for (int h = 0; h < height_; h++) {
			MapCell* cell = new MapCell();
			cell->position_.x = static_cast<float>(w);
			cell->position_.y = static_cast<float>(h);
			cell->resources.clear(); // Initialize empty resources list

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

int Map::getHeight() const {
	return height_;
}

int Map::getWidth() const {
	return width_;
}

MapCell* Map::getCellAt(int x, int y) {
	if (x >= 0 && y >= 0 && x < width_ && y < height_)
		return cost_map_[x][y];

	return nullptr;
}

const MapCell* Map::getCellAt(int x, int y) const {
	if (x >= 0 && y >= 0 && x < width_ && y < height_)
		return cost_map_[x][y];

	return nullptr;
}

bool Map::isWalkable(MOMOS::Vec2 position) const {
		const MapCell* cell = getCellAt(static_cast<int>(position.x), static_cast<int>(position.y));
	return cell ? cell->isWalkable() : false;
}

::MOMOS::Vec2 Map::ScreenToMapCoords(MOMOS::Vec2 pos) const {
	MOMOS::Vec2 map_coords;
	map_coords.x = (pos.x > 0.0f) ? (float)((int)round(pos.x / ((float)Screen::width / (float)width_)) % width_) : 0;
	map_coords.y = (pos.y > 0.0f) ? (float)((int)round(pos.y / ((float)Screen::height / (float)height_)) % height_) : 0;

	return map_coords;
}

::MOMOS::Vec2 Map::MapToScreenCoords(::MOMOS::Vec2 pos) const {
	MOMOS::Vec2 scr_coords;
	scr_coords.x = pos.x * ((float)Screen::width / (float)width_);
	scr_coords.y = pos.y * ((float)Screen::height / (float)height_);

	return scr_coords;
}

bool Map::isWalkableInScreenCoords(MOMOS::Vec2 pos) const {
	return isWalkable(ScreenToMapCoords(pos));
}

void Map::Print() const {
	for (int i = 0; i < width_; i++) {
		printf("\n");
		for (int j = 0; j < height_; j++) {
			if (cost_map_[i][j]) {
				printf("%d  ", cost_map_[i][j]->f);
			} else {
				printf("?  ");
			}
		}
	}
}

bool Map::HandleCellClick(const ::MOMOS::Vec2& screen_pos) {
	if (!MOMOS::MouseButtonDown(1)) {
		return false;
	}

	// Convert screen coordinates to world coordinates (accounting for camera)
	::MOMOS::Vec2 world_click = Camera::ScreenToWorld(screen_pos);

	// Calculate tile world dimensions (same as in MapRenderer::Draw)
	float tile_world_width = static_cast<float>(Screen::width) / static_cast<float>(width_);
	float tile_world_height = static_cast<float>(Screen::height) / static_cast<float>(height_);

	// Convert world coordinates to map coordinates
	int map_x = static_cast<int>(world_click.x / tile_world_width);
	int map_y = static_cast<int>(world_click.y / tile_world_height);

	// Clamp to valid map bounds
	map_x = std::max(0, std::min(map_x, width_ - 1));
	map_y = std::max(0, std::min(map_y, height_ - 1));

	// Get the cell at the clicked position
		const MapCell* cell = getCellAt(map_x, map_y);
	if (cell == nullptr) {
		return false;
	}

	// Store selected cell position
	selected_cell_ = { static_cast<float>(map_x), static_cast<float>(map_y) };

	// Format and display the cell cost
	char cost_text[64];
	snprintf(cost_text, sizeof(cost_text), "Cell Cost: %.2f", cell->cost_);
	InfoPanel::Get().SetMessage(cost_text);
	
	return true;
}

void Map::DrawCellSelection() const {
	// Check if a cell is selected (valid position)
	if (selected_cell_.x < 0.0f || selected_cell_.y < 0.0f) {
		return;
	}

	// Calculate tile world dimensions
	float tile_world_width = static_cast<float>(Screen::width) / static_cast<float>(width_);
	float tile_world_height = static_cast<float>(Screen::height) / static_cast<float>(height_);

	// Get cell position in world coordinates
	int cell_x = static_cast<int>(selected_cell_.x);
	int cell_y = static_cast<int>(selected_cell_.y);

	// Calculate world bounds of the cell
	::MOMOS::Vec2 world_top_left = {
		cell_x * tile_world_width,
		cell_y * tile_world_height
	};
	::MOMOS::Vec2 world_bottom_right = {
		(cell_x + 1) * tile_world_width,
		(cell_y + 1) * tile_world_height
	};

	// Convert to screen coordinates
	::MOMOS::Vec2 top_left_screen = Camera::WorldToScreen(world_top_left);
	::MOMOS::Vec2 bottom_right_screen = Camera::WorldToScreen(world_bottom_right);

	// Draw green square around the cell
	MOMOS::DrawSetStrokeColor(100, 255, 100, 255);
	MOMOS::DrawLine(top_left_screen.x, top_left_screen.y, bottom_right_screen.x, top_left_screen.y);
	MOMOS::DrawLine(bottom_right_screen.x, top_left_screen.y, bottom_right_screen.x, bottom_right_screen.y);
	MOMOS::DrawLine(bottom_right_screen.x, bottom_right_screen.y, top_left_screen.x, bottom_right_screen.y);
	MOMOS::DrawLine(top_left_screen.x, bottom_right_screen.y, top_left_screen.x, top_left_screen.y);
}

::MOMOS::Vec2 Map::GetSelectedCell() const {
	return selected_cell_;
}

void Map::ClearCellSelection() {
	selected_cell_ = { -1.0f, -1.0f };
}

