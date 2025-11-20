/*
* File:   Door.cc
* Author: Marcos Vazquez
*
* Created on May 9, 2017, 12:26 PM
*/

#include "../include/Door.h"
#include "../include/GameStatus.h"
#include "../include/Agents/Pathfinder.h"

Door::Door(float x, float y, bool vertical) {
	pos_ = { x, y };
	is_vertical_ = vertical;
	is_open_ = true;
	locked_open_time_ = 0;

	if (is_vertical_) {
		img_open_ = MOMOS::SpriteFromFile("data/door_v_open.bmp");
		img_closed_ = MOMOS::SpriteFromFile("data/door_v_closed.bmp");
	} else {
		img_open_ = MOMOS::SpriteFromFile("data/door_h_open.bmp");
		img_closed_ = MOMOS::SpriteFromFile("data/door_h_closed.bmp");
	}
};


Door::~Door() {
	MOMOS::SpriteRelease(img_open_);
	MOMOS::SpriteRelease(img_closed_);
};


MOMOS::SpriteHandle Door::getImg() {
	return (is_open_) ? img_open_ : img_closed_;
}


bool Door::open() {
	CostMap* map = GameStatus::get()->map;
	if (is_vertical_) {
		map->getCellAt((int)pos_.x + 0, (int)pos_.y + 0)->is_walkable_ = true;
		map->getCellAt((int)pos_.x + 0, (int)pos_.y + 1)->is_walkable_ = true;
	}
	else {
		map->getCellAt((int)pos_.x + 0, (int)pos_.y + 0)->is_walkable_ = true;
		map->getCellAt((int)pos_.x + 1, (int)pos_.y + 0)->is_walkable_ = true;
	}

	//Invalidate precalculated paths
	if (!is_open_)
		GameStatus::get()->pathfinder_->clearCachedPaths();

	is_open_ = true;
	return true;
}

bool Door::close() {
	CostMap* map = GameStatus::get()->map;

	if (GameStatus::get()->game_time > locked_open_time_) {
		if (is_vertical_) {
			map->getCellAt((int)pos_.x + 0, (int)pos_.y + 0)->is_walkable_ = false;
			map->getCellAt((int)pos_.x + 0, (int)pos_.y + 1)->is_walkable_ = false;
		} else {
			map->getCellAt((int)pos_.x + 0, (int)pos_.y + 0)->is_walkable_ = false;
			map->getCellAt((int)pos_.x + 1, (int)pos_.y + 0)->is_walkable_ = false;
		}
	
		//Invalidate precalculated paths
		if (is_open_)
			GameStatus::get()->pathfinder_->clearCachedPaths();
		
		is_open_ = false;
	}


	return true;
}


MOMOS::Vec2 Door::getFrontalPoint(bool in) {
	MOMOS::Vec2 p;
	int dir = (in) ? 1 : -1;
	if (is_vertical_) {
		p = { pos_.x - dir, pos_.y };
	} else {
		p = { pos_.x, pos_.y + dir };
	}
	return p;
}