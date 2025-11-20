#include "../../include/Pathfinding/path.h"
#include "../../include/Pathfinding/cost_map.h"

Path::Path() {
	current_point_ = 0;
}


Path::Path(const Path& orig) {}
Path::~Path() {}


bool Path::Add(MOMOS::Vec2 position) {
	path_.push_back(position);
	return true;
}


int Path::Length() {
	return path_.size();
}


MOMOS::Vec2 Path::NextPoint() {
	return path_[current_point_ + 1];
}


void Path::Print(CostMap *map) {
	MOMOS::SpriteHandle sign = MOMOS::SpriteFromFile("data/bluedot.png");
	
	for (unsigned int i = 0; i < path_.size(); i++) {
		MOMOS::Vec2 src_coords = map->MapToScreenCoords(path_[i]);
		MOMOS::DrawSprite(sign, src_coords.x, src_coords.y);
	}

	MOMOS::SpriteRelease(sign);
}