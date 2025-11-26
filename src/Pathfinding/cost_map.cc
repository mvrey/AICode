#include "../../include/Pathfinding/cost_map.h"
#include "../../include/Camera.h"

#include <cstdlib>

namespace {

::MOMOS::SpriteHandle CreatePixelSprite(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	unsigned char pixel[4] = { r, g, b, a };
	return MOMOS::SpriteFromMemory(1, 1, pixel);
}

} // namespace

CostMap::CostMap() {
	width_ = 0;
	height_ = 0;
	tile_sprite_ = CreatePixelSprite(255, 255, 255, 255);
	blocked_tile_sprite_ = CreatePixelSprite(0, 0, 0, 255);
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

	float clamped_ratio = blocked_ratio;
	if (clamped_ratio < 0.0f) {
		clamped_ratio = 0.0f;
	} else if (clamped_ratio > 1.0f) {
		clamped_ratio = 1.0f;
	}
	int threshold = static_cast<int>(clamped_ratio * 10000.0f);

	for (int x = 0; x < width_; ++x) {
		for (int y = 0; y < height_; ++y) {
			bool blocked = (rand() % 10000) < threshold;
			tile_walkable_[x][y] = !blocked;
		}
	}

	tile_walkable_[0][0] = true;
	if (width_ > 1) {
		tile_walkable_[1][0] = true;
	}
	if (height_ > 1) {
		tile_walkable_[0][1] = true;
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
			if (w < static_cast<int>(tile_walkable_.size()) && h < static_cast<int>(tile_walkable_[w].size())) {
				walkable = tile_walkable_[w][h];
			}
			cell->cost_ = walkable ? 0.0f : 1.0f;
			cell->is_walkable_ = walkable;

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
	return getCellAt(static_cast<int>(position.x), static_cast<int>(position.y))->is_walkable_;
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
	if (width_ <= 0 || height_ <= 0 || tile_sprite_ == nullptr || blocked_tile_sprite_ == nullptr) {
		return;
	}

	float tile_world_width = static_cast<float>(Screen::width) / static_cast<float>(width_);
	float tile_world_height = static_cast<float>(Screen::height) / static_cast<float>(height_);

	for (int x = 0; x < width_; ++x) {
		for (int y = 0; y < height_; ++y) {
			::MOMOS::Vec2 world_top_left = { x * tile_world_width, y * tile_world_height };
			::MOMOS::Vec2 world_bottom_right = { (x + 1) * tile_world_width, (y + 1) * tile_world_height };
			::MOMOS::Vec2 screen_top_left = Camera::WorldToScreen(world_top_left);
			::MOMOS::Vec2 screen_bottom_right = Camera::WorldToScreen(world_bottom_right);

			::MOMOS::SpriteTransform sprite_transform{};
			sprite_transform.x = screen_top_left.x;
			sprite_transform.y = screen_top_left.y;
			sprite_transform.scale_x = screen_bottom_right.x - screen_top_left.x;
			sprite_transform.scale_y = screen_bottom_right.y - screen_top_left.y;

			bool walkable = true;
			if (x < static_cast<int>(tile_walkable_.size()) && y < static_cast<int>(tile_walkable_[x].size())) {
				walkable = tile_walkable_[x][y];
			}

			MOMOS::DrawSprite(walkable ? tile_sprite_ : blocked_tile_sprite_, sprite_transform);
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