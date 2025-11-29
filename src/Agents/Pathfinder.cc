#include "../../include/Agents/Pathfinder.h"
#include "../../include/Map/Map.h"

/*****************************/
/************ AGENT **********/
/*****************************/


Pathfinder::Pathfinder() {
	mind_ = new PathfinderMind();
	mind_->owner_ = this;
	map_ = nullptr;
	astar_ = nullptr;
}


Pathfinder::Pathfinder(const Pathfinder& orig) {

}


Pathfinder::~Pathfinder() {

}


void Pathfinder::init(Map* map) {
	map_ = map;
	astar_ = new AStar();
	if (map) {
		astar_->PreProcess(map);
	}
}


void Pathfinder::update(double accumTime) {
	if (true) {
		mind_->update(accumTime);
	}
}


/*****************************/
/************ MIND ***********/
/*****************************/


void PathfinderMind::update(double accumTime) {
	this->sense();
	this->reason();
}


void PathfinderMind::sense() {
}


/// Switches over status and commands actions to the body
using PathKey = std::size_t;

static PathKey MakePathKey(const ::MOMOS::Vec2& start, const ::MOMOS::Vec2& end) {
	int sx = static_cast<int>(start.x);
	int sy = static_cast<int>(start.y);
	int dx = static_cast<int>(end.x);
	int dy = static_cast<int>(end.y);
	return (static_cast<PathKey>(sx & 0xffff) << 48) |
		   (static_cast<PathKey>(sy & 0xffff) << 32) |
		   (static_cast<PathKey>(dx & 0xffff) << 16) |
		   static_cast<PathKey>(dy & 0xffff);
}

void PathfinderMind::reason() {
	//Process A* queue
	if (owner_->commands_.empty()) {
		return;
	}

	Map* map = owner_->map_;
	if (!map) {
		return;
	}

	PathCommand* command = owner_->commands_.front();
	PathKey key = MakePathKey(command->start, command->end);

	auto cached = owner_->cached_paths_.find(key);
	if (cached != owner_->cached_paths_.end()) {
		command->path_->path_ = cached->second;
		command->calculated = true;
		command->pending_ = false;
		owner_->calc_paths_.push_back(command);
		owner_->commands_.erase(owner_->commands_.begin());
		return;
	}

	map->reset();
	owner_->astar_->GeneratePath(command->start, command->end, command->path_);
	command->calculated = true;
	command->pending_ = false;

	owner_->calc_paths_.push_back(command);
	owner_->cached_paths_[key] = command->path_->path_;
	owner_->commands_.erase(owner_->commands_.begin());
}


void Pathfinder::search(PathCommand* cmd) {
	commands_.push_back(cmd);
}


void Pathfinder::clearCachedPaths() {
	calc_paths_.clear();
}