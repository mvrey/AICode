#include "../../include/Pathfinding/Map.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

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
		for (Cell* cell : column) {
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

int Map::getHeight() const {
	return height_;
}

int Map::getWidth() const {
	return width_;
}

Cell* Map::getCellAt(int x, int y) {
	if (x >= 0 && y >= 0 && x < width_ && y < height_)
		return cost_map_[x][y];

	return nullptr;
}

const Cell* Map::getCellAt(int x, int y) const {
	if (x >= 0 && y >= 0 && x < width_ && y < height_)
		return cost_map_[x][y];

	return nullptr;
}

bool Map::isWalkable(MOMOS::Vec2 position) const {
	const Cell* cell = getCellAt(static_cast<int>(position.x), static_cast<int>(position.y));
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

