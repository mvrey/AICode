#include "../../include/Pathfinding/path.h"
#include "../../include/Pathfinding/cost_map.h"
#include "../../include/Camera.h"

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
	float zoom = Camera::Zoom();
	float half_width = MOMOS::SpriteWidth(sign) * 0.5f * zoom;
	float half_height = MOMOS::SpriteHeight(sign) * 0.5f * zoom;
	::MOMOS::SpriteTransform sprite_transform{};

	for (unsigned int i = 0; i < path_.size(); i++) {
		::MOMOS::Vec2 world_coords = map->MapToScreenCoords(path_[i]);
		::MOMOS::Vec2 screen_coords = Camera::WorldToScreen(world_coords);
		sprite_transform.x = screen_coords.x - half_width;
		sprite_transform.y = screen_coords.y - half_height;
		sprite_transform.scale_x = zoom;
		sprite_transform.scale_y = zoom;
		MOMOS::DrawSprite(sign, sprite_transform);
	}

	MOMOS::SpriteRelease(sign);
}