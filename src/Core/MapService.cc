//------------------------------------------------------------------------------
// File: MapService.cc
// Purpose: Implementation of MapService
//------------------------------------------------------------------------------
#include "../../include/Core/MapService.h"
#include "../../include/Map/Map.h"

MapService::MapService()
	: map_(nullptr)
{
}

MapService::~MapService() {
	delete map_;
}

void MapService::SetMap(Map* map) {
	delete map_;
	map_ = map;
}

