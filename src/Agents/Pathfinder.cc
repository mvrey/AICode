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

		/// PRE-CALCULATED PATHS CURRENTLY DISABLED DUE TO THEM GIVING MORE TROUBLE THAN THEY'RE WORTH
		//Search for this path being pre-calculated
		/*
		for (int i = 0; i < owner_->calc_paths_.size(); i++) {
			if (owner_->commands_[0]->start.x == owner_->calc_paths_[i]->start.x && owner_->commands_[0]->start.y == owner_->calc_paths_[i]->start.y &&
				owner_->commands_[0]->end.x == owner_->calc_paths_[i]->end.x && owner_->commands_[0]->end.y == owner_->calc_paths_[i]->end.y) {

				owner_->commands_[0]->path_ = owner_->calc_paths_[i]->path_;
				owner_->commands_[0]->calculated = true;
				owner_->commands_[0]->pending_ = false;
				owner_->commands_.erase(owner_->commands_.begin());
				printf("RETURN PRECALC PATH");
				return;
			}
		}
		*/

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