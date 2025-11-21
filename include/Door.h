/*
* File:   Door.h
* Author: Marcos Vazquez
*
* Created on May 9, 2017, 12:26 PM
*
*/

#ifndef DOOR_H
#define DOOR_H

#include "config.h"


class Door {
public:
	Door(float x, float y, bool vertical);
	Door(const Door& orig) {};
	~Door();

	bool open();
	bool close();

	::MOMOS::SpriteHandle getImg();
	///Returns a map point right in front of the door (inside or outside the prison)
	::MOMOS::Vec2 getFrontalPoint(bool in = true);

	::MOMOS::SpriteHandle img_open_;
	::MOMOS::SpriteHandle img_closed_;
	
	bool is_vertical_;
	bool is_open_ = false;
	//Until this time, the door cannot be closed
	double locked_open_time_;

	//In map coordinates
	MOMOS::Vec2 pos_;
	
private:
};

#endif /* DOOR_H */