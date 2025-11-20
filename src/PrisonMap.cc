/*
* File:   PrisonMap.cc
* Author: Marcos Vazquez
*
* Created on May 7, 2017, 16:05 PM
*/

#include "../include/PrisonMap.h"


PrisonMap::PrisonMap() {
	area_map_handle_ = MOMOS::SpriteFromFile("data/map_03_960x704_color.bmp");

	//Corners are listed starting top-left and rotating counter-clockwise

	/// RESTING ROOM
	resting_room_.corners_.push_back({ 34, 30 });
	resting_room_.corners_.push_back({ 48, 30 });
	resting_room_.corners_.push_back({ 48, 33 });
	resting_room_.corners_.push_back({ 34, 33 });
	resting_room_.id_ = 0;
	resting_room_.name_ = "RESTROOM";
	rooms_.push_back(&resting_room_);

	/// WORKING AREA
	working_area_.corners_.push_back({ 24, 14 });
	working_area_.corners_.push_back({ 42, 14 });
	working_area_.corners_.push_back({ 42, 20 });
	working_area_.corners_.push_back({ 24, 20 });

	/// Mark Loading/Unloading areas to be a corner fraction of the working area
	int area_width = (int)(working_area_.corners_[1].x - working_area_.corners_[0].x);
	int area_height = (int)(working_area_.corners_[2].y - working_area_.corners_[0].y);
	
	/// LOADING AREA
	loading_area_.corners_.push_back(working_area_.corners_[0]);
	loading_area_.corners_.push_back(MOMOS::Vec2 { working_area_.corners_[0].x + (int)(area_width / 5.0f), working_area_.corners_[0].y });
	loading_area_.corners_.push_back(MOMOS::Vec2 { working_area_.corners_[0].x + (int)(area_width / 5.0f), working_area_.corners_[2].y });
	loading_area_.corners_.push_back(working_area_.corners_[3]);
	loading_area_.id_ = 2;
	loading_area_.name_ = "LOADAREA";
	rooms_.push_back(&loading_area_);

	/// UNLOADING AREA
	unloading_area_.corners_.push_back(MOMOS::Vec2{ working_area_.corners_[1].x - (int)(area_width / 5.0f), working_area_.corners_[0].y });
	unloading_area_.corners_.push_back(working_area_.corners_[1]);
	unloading_area_.corners_.push_back(working_area_.corners_[2]);
	unloading_area_.corners_.push_back(MOMOS::Vec2{ working_area_.corners_[2].x - (int)(area_width / 5.0f), working_area_.corners_[2].y });
	unloading_area_.id_ = 3;
	unloading_area_.name_ = "UNLOADAREA";
	rooms_.push_back(&unloading_area_);


	/// THIS MUST BE ADDED THE LAST NOT TO OVERLAP ITS SUBAREAS POSITIONS
	working_area_.id_ = 1;
	working_area_.name_ = "WORKAREA";
	rooms_.push_back(&working_area_);
	
	/// RIGHT ABOVE RESTING ROOM
	pre_resting_room_.corners_.push_back({ 26, 25 });
	pre_resting_room_.corners_.push_back({ 41, 25 });
	pre_resting_room_.corners_.push_back({ 41, 28 });
	pre_resting_room_.corners_.push_back({ 26, 28 });
	pre_resting_room_.id_ = 4;
	pre_resting_room_.name_ = "PRERESTING";
	rooms_.push_back(&pre_resting_room_);

	/// RIGHT ABOVE WORKING AREA
	up1_working_area_.corners_.push_back({ 25, 10 });
	up1_working_area_.corners_.push_back({ 42, 10 });
	up1_working_area_.corners_.push_back({ 42, 12 });
	up1_working_area_.corners_.push_back({ 25, 12 });
	up1_working_area_.id_ = 5;
	up1_working_area_.name_ = "WORKUP1";
	rooms_.push_back(&up1_working_area_);

	/// TOP ABOVE WORKING AREA
	up2_working_area_.corners_.push_back({ 25, 6 });
	up2_working_area_.corners_.push_back({ 42, 6 });
	up2_working_area_.corners_.push_back({ 42, 9 });
	up2_working_area_.corners_.push_back({ 25, 9 });
	up2_working_area_.id_ = 6;
	up2_working_area_.name_ = "WORKUP2";
	rooms_.push_back(&up2_working_area_);


	/// DOORS
	Door* d = new Door(9, 14, false);
	doors_.push_back(d);
	d = new Door(51, 16, true);
	doors_.push_back(d);

	/// WAYPOINTS
	//pre-resting left door
	MOMOS::Vec2 w = { 29, 24 };
	waypoints_.push_back(w);
	//pre-resting right door
	w = { 38, 24 };
	waypoints_.push_back(w);
	//resting area door
	w = { 35, 29 };
	waypoints_.push_back(w);

	//workup1 middle door
	w = { 33, 12 };
	waypoints_.push_back(w);
	//workup2 left door
	w = { 29, 9 };
	waypoints_.push_back(w);
	//workup2 right door
	w = { 38, 9 };
	waypoints_.push_back(w);
}


PrisonMap::~PrisonMap() {
	//Release sprite handle 
	MOMOS::SpriteRelease(area_map_handle_);
}


PrisonAreaType PrisonMap::getAreaTypeAt(MOMOS::Vec2 area_coords) {
	int height = MOMOS::SpriteHeight(area_map_handle_);
	int width = MOMOS::SpriteWidth(area_map_handle_);

	//Screen to map coordinates
	MOMOS::Vec2 map_coords;
	map_coords.x = (area_coords.x > 0.0f) ? (float)((int)(area_coords.x / ((float)Screen::width / (float)width)) % width) : 0;
	map_coords.y = (area_coords.y > 0.0f) ? (float)((int)(area_coords.y / ((float)Screen::height / (float)height)) % height) : 0;

	//Get pixel at given map coordinates
	unsigned char pixel[4];
	MOMOS::SpriteGetPixel(area_map_handle_, (int)map_coords.x, (int)map_coords.y, pixel);

	//Compose hexadecimal color code in RGB format. Ignore alpha channel.
	int color = pixel[0]<<16 | pixel[1]<<8 | pixel[2]<<0;

	//Compare pixel values to return corresponding are type
	PrisonAreaType area;
	switch (color) {
	case 0x00A2E8:
		area = kRiver;
		break;
	case 0x544B40:
		area = kBridge;
	case 0XA28356:
		area = kGround;
		break;
	case 0XD0734F:
		area = kBuilding;
		break;
	case 0X545454:
		area = kCourtyard;
		break;
	case 0X586A1C:
		area = kYard;
		break;
	case 0X009BDB:
	case 0X006F9B:
		area = kBase;
		break;
	case 0XDC0305:
		area = kDoor;
		break;
	case 0X000000:
		area = kWall;
	default:
		area = kNONE;
		break;
	}

	//Return area type
	return area;
}


Room* PrisonMap::getRoomAt(MOMOS::Vec2 coords) {
	Room* found = nullptr;

	for (unsigned int i = 0; i < rooms_.size() && found==nullptr; i++) {
		if (coords.x >= rooms_[i]->corners_[0].x && coords.x <= rooms_[i]->corners_[1].x &&
			coords.y >= rooms_[i]->corners_[0].y && coords.y <= rooms_[i]->corners_[2].y)
		found = rooms_[i];
	}

	return found;
}


MOMOS::Vec2 PrisonMap::getRandomPointInRoom(Room room) {
	int x = (int)room.corners_[0].x + (rand() % (int)(room.corners_[1].x - room.corners_[0].x));
	int y = (int)room.corners_[0].y + (rand() % (int)(room.corners_[2].y - room.corners_[0].y));

	MOMOS::Vec2 rt = { (float)x, (float)y };
	return rt;
}


std::vector<MOMOS::Vec2> PrisonMap::getPathToRoom(Room* start, Room* end) {
	
	std::vector<MOMOS::Vec2> points;

	switch (start->id_) {
	//RESTING
	case 0:
		//If going to loading area
		if (end->id_ == 2) {
			points.push_back(waypoints_[2]);
			points.push_back(waypoints_[0]);
		}
		break;
	//WORKING
	case 1:
	//LOADING
	case 2:
	//UNLOADING
	case 3:
		if (end->id_ == 0) {
		//if going to rest area
			points.push_back(waypoints_[0]);
			points.push_back(waypoints_[2]);
		} else if (end->id_ == 1 || end->id_ == 2 || end->id_ == 3) {
		//if going anywhere else in the working area. Fill with something.
			points.push_back(getRandomPointInRoom(*end));
		}
		break;
	//PRERESTING
	case 4:
		if (end->id_ == 0) {
			//if going to rest area
			points.push_back(waypoints_[2]);
		} else if (end->id_ == 1 || end->id_ == 2 || end->id_ == 3) {
			//if going anywhere in the working area.
			points.push_back(waypoints_[1]);
		}
		break;
	//WORKUP1
	case 5:
		if (end->id_ == 6) {
			//if going to workup2
			points.push_back(waypoints_[4]);
		} else if (end->id_ == 1 || end->id_ == 2 || end->id_ == 3) {
			//if going anywhere in the working area.
			points.push_back(waypoints_[3]);
		}
		break;
	//WORKUP2
	case 6:
		if (end->id_ == 5) {
			//if going to workup1
			points.push_back(waypoints_[4]);
		}
		else if (end->id_ == 1 || end->id_ == 2 || end->id_ == 3) {
			//if going anywhere in the working area.
			points.push_back(waypoints_[4]);
			points.push_back(waypoints_[3]);
		}
		break;
	default:
		break;
	}

	return points;
}