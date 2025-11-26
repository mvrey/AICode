/*
* A manager that feeds on pathfinding request and processes them at his own pace.
* Helps control CPU usage by caching paths and stablishing the rate at which paths are searched.
*/
#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <cstdlib>
#include "Agent.h"
#include "../Pathfinding/astar.h"
#include <unordered_map>

class Pathfinder;

//Psychic attributes. Managers are mind-pure.
class PathfinderMind : public AgentMind {

friend class Pathfinder;

public:
	virtual void sense() override;
	virtual void reason() override;
	void update(double accumTime) override;

	Agent* target_;
	Pathfinder* owner_;
};


/// A class that holds a pathfinding request
class PathCommand {
public:
	PathCommand() { path_ = new Path(); }
	//Start and end points to calculate
	::MOMOS::Vec2 start, end;
	//If the path has been already calculates
	bool calculated = false;
	//If the path is waiting in line to be calculates
	bool pending_ = true;
	//Result of the pathfinding calculation is held here
	Path* path_;
};


class Pathfinder : public Agent {
public:

	Pathfinder();
	Pathfinder(const Pathfinder& orig);
	virtual ~Pathfinder();

	void init();
	///Processes PathCommand queue
	void update(double accumTime) override;
	///Apends a PathCommand to the queue
	void search(PathCommand* cmd);
	///Clears the pre-calculated paths
	void clearCachedPaths();

	PathfinderMind* mind_;

	AStar* astar_;
	std::vector<PathCommand*> commands_;

	//Cached paths
	std::vector<PathCommand*> calc_paths_;
	// Cached vectors keyed by (start,end) packed coordinates
	std::unordered_map<std::size_t, std::vector<::MOMOS::Vec2>> cached_paths_;

private:
	
};

#endif /* PATHFINDER_H */