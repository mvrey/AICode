#include "../../include/Pathfinding/cost_map.h"

CostMap::CostMap() {
	//handle_ = new MOMOS::SpriteHandle();
}

CostMap::CostMap(const CostMap& orig) {}
CostMap::~CostMap() {}


bool CostMap::Load(const char *cost_img, const char *terrain_img) {
	cost_img_handle_ = MOMOS::SpriteFromFile(cost_img);
	terrain_img_handle_ = MOMOS::SpriteFromFile(terrain_img);

	height_ = MOMOS::SpriteHeight(cost_img_handle_);
	width_ = MOMOS::SpriteWidth(cost_img_handle_);

	reset();
	

	return true;
}

void CostMap::InitializeSynthetic(int width, int height, bool walkable) {
	for (auto& column : cost_map_) {
		for (Cell* cell : column) {
			delete cell;
		}
	}
	cost_map_.clear();

	width_ = (width > 0) ? width : 1;
	height_ = (height > 0) ? height : 1;

	cost_map_.resize(width_);
	for (int x = 0; x < width_; ++x) {
		cost_map_[x].reserve(height_);
		for (int y = 0; y < height_; ++y) {
			Cell* cell = new Cell();
			cell->position_.x = static_cast<float>(x);
			cell->position_.y = static_cast<float>(y);
			cell->is_walkable_ = walkable;
			cell->cost_ = walkable ? 0.0f : 1.0f;
			cost_map_[x].push_back(cell);
		}
	}
}


void CostMap::reset() {
	
	cost_map_.clear();
	
	for (int w = 0; w < width_; w++) {
		cost_map_.push_back(std::vector<Cell*>());

		for (int h = 0; h < height_; h++) {
			unsigned char outRGBA[4];
			MOMOS::SpriteGetPixel(cost_img_handle_, w, h, outRGBA);

			//Just check the red color channel, since G&B will be the same
			Cell* cell = new Cell();
			cell->position_.x = (float)w;
			cell->position_.y = (float)h;
			cell->cost_ = (outRGBA[0] == 0) ? 1.0f : 0.0f;
			cell->is_walkable_ = (outRGBA[0] == 0) ? false : true;

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
	return getCellAt((int)position.x, (int)position.y)->is_walkable_;
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
	MOMOS::SpriteTransform *trans = new MOMOS::SpriteTransform();

	trans->scale_x = (float)Screen::width / (float)MOMOS::SpriteWidth(terrain_img_handle_);
	trans->scale_y = (float)Screen::height / (float)MOMOS::SpriteHeight(terrain_img_handle_);

	MOMOS::DrawSprite(terrain_img_handle_, *trans);
}