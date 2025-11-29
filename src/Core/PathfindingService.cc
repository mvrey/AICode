//------------------------------------------------------------------------------
// File: PathfindingService.cc
// Purpose: Implementation of PathfindingService
//------------------------------------------------------------------------------
#include "../../include/Core/PathfindingService.h"
#include "../../include/Agents/Pathfinder.h"

PathfindingService::PathfindingService()
	: pathfinder_(nullptr)
{
}

PathfindingService::~PathfindingService() {
	delete pathfinder_;
}

void PathfindingService::SetPathfinder(Pathfinder* pathfinder) {
	delete pathfinder_;
	pathfinder_ = pathfinder;
}

