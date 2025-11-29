//------------------------------------------------------------------------------
// File: PathfindingService.h
// Purpose: Manages the pathfinding system
//------------------------------------------------------------------------------
#ifndef PATHFINDING_SERVICE_H
#define PATHFINDING_SERVICE_H

class Pathfinder;

/// Service that manages the pathfinding system
class PathfindingService {
public:
	PathfindingService();
	~PathfindingService();
	
	/// Set the pathfinder (takes ownership)
	void SetPathfinder(Pathfinder* pathfinder);
	
	/// Get the pathfinder (non-owning pointer)
	Pathfinder* GetPathfinder() { return pathfinder_; }
	const Pathfinder* GetPathfinder() const { return pathfinder_; }
	
	/// Check if pathfinder is available
	bool HasPathfinder() const { return pathfinder_ != nullptr; }

private:
	Pathfinder* pathfinder_;
};

#endif // PATHFINDING_SERVICE_H

