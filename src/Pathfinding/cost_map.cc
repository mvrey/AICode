#include "../../include/Pathfinding/cost_map.h"

CostMap::CostMap() {
}

CostMap::CostMap(const CostMap& orig) {
	// Copy not fully implemented - would need to copy map data
}

CostMap::~CostMap() {
	// Destructors of member objects handle cleanup
}

bool CostMap::Load(const char *cost_img, const char *terrain_img) {
	return generator_.Load(map_, cost_img, terrain_img);
}

bool CostMap::GenerateTileMap(int cols, int rows, float blocked_ratio) {
	return generator_.GenerateTileMap(map_, cols, rows, blocked_ratio);
}

void CostMap::InitializeSynthetic(int width, int height, bool walkable) {
	generator_.InitializeSynthetic(map_, width, height, walkable);
}

void CostMap::reset() {
	map_.reset();
}

int CostMap::getHeight() {
	return map_.getHeight();
}

int CostMap::getWidth() {
	return map_.getWidth();
}

Cell* CostMap::getCellAt(int x, int y) {
	return map_.getCellAt(x, y);
}

bool CostMap::isWalkable(::MOMOS::Vec2 position) {
	return map_.isWalkable(position);
}

::MOMOS::Vec2 CostMap::ScreenToMapCoords(MOMOS::Vec2 pos) {
	return map_.ScreenToMapCoords(pos);
}

::MOMOS::Vec2 CostMap::MapToScreenCoords(::MOMOS::Vec2 pos) {
	return map_.MapToScreenCoords(pos);
}

bool CostMap::isWalkableInScreenCoords(::MOMOS::Vec2 pos) {
	return map_.isWalkableInScreenCoords(pos);
}

void CostMap::Print() {
	map_.Print();
}

void CostMap::Draw() {
	renderer_.Draw(map_);
}
