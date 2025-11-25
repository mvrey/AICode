#include "../../include/Agents/Pathfinder.h"

/*****************************/
/************ AGENT **********/
/*****************************/


Pathfinder::Pathfinder() {
	mind_ = new PathfinderMind();
	mind_->owner_ = this;

	init();
}


Pathfinder::Pathfinder(const Pathfinder& orig) {

}


Pathfinder::~Pathfinder() {

}


void Pathfinder::init() {
	astar_ = new AStar();
	astar_->PreProcess(GameStatus::get()->map);
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
void PathfinderMind::reason() {
	//Process A* queue
	if (owner_->commands_.size() > 0) {

		CostMap* map = GameStatus::get()->map;

		AStar* astar = new AStar();

		astar->PreProcess(map);

		GameStatus::get()->map->reset();

		owner_->astar_->GeneratePath(owner_->commands_[0]->start, owner_->commands_[0]->end, owner_->commands_[0]->path_);
		owner_->commands_[0]->calculated = true;
		owner_->commands_[0]->pending_ = false;

		//Add to pre-calculated paths
		owner_->calc_paths_.push_back(owner_->commands_[0]);

		//Remove from queue
		owner_->commands_.erase(owner_->commands_.begin());
	}
}


void Pathfinder::search(PathCommand* cmd) {
	commands_.push_back(cmd);
}


void Pathfinder::clearCachedPaths() {
	calc_paths_.clear();
}