/*
* File:   PrisonMap.h
* Author: Marcos Vazquez
*
* Created on May 7, 2017, 16:05 PM
* 
* Holds information about the positioning of elements in the current prison map
*/

#ifndef PRISONMAP_H
#define PRISONMAP_H

#include "config.h"
#include "Crate.h"
#include "Door.h"

typedef struct {
	unsigned int id_;
	std::string name_;
	std::vector<::MOMOS::Vec2> corners_;
} Room;


class PrisonMap {

public:
	PrisonMap();
	PrisonMap(const PrisonMap& orig) {};
	virtual ~PrisonMap();

	///Returns the type of area that lies on the given coordinates, based on the sprite's color
	PrisonAreaType getAreaTypeAt(::MOMOS::Vec2 area_coords);

	///Returns the room in a given coordinate
	Room* getRoomAt(::MOMOS::Vec2 coords);

	///Returns a map random point in a given room
	::MOMOS::Vec2 getRandomPointInRoom(Room room);
	
	///Calculates a path between 2 rooms using waypoints
	std::vector<::MOMOS::Vec2> getPathToRoom(Room* start, Room* end);

	::MOMOS::SpriteHandle area_map_handle_;

	//Doors
	std::vector<Door*> doors_;

	//Room limits in map coordinates
	Room resting_room_;
	Room working_area_;
	Room loading_area_;
	Room unloading_area_;
	
	//The following rooms are not used by prisoners on their duties
	Room pre_resting_room_;
	Room up1_working_area_;
	Room up2_working_area_;

	//Holds all the rooms
	std::vector<Room*> rooms_;

	//Holds all the waypoints
	std::vector<::MOMOS::Vec2> waypoints_;
};

#endif /* PRISONMAP_H */