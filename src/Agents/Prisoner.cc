#include "../../include/Agents/Prisoner.h"

/*****************************/
/************ AGENT **********/
/*****************************/


Prisoner::Prisoner() {
	body_ = new PrisonerBody();
	mind_ = new PrisonerMind();
	body_->owner_ = this;
	mind_->owner_ = this;

	body_->pos_ = { Screen::width / 2, Screen::height / 2 };
	body_->direction_ = { -1.0f, 0.0f };

	last_movement_update_ = 0.0f;
	movement_threshold_ = 3000.0;
	mind_->status_ = kGoingToWork;

	working_shift_ = 0;

	speed_ = 0.1f * GameStatus::get()->simulation_speed_;
	original_speed_ = speed_;

	pattern_steps_ = { kGoLeft, kGoDown, kGoUp, kGoRight, kStop };

	init();
}


Prisoner::Prisoner(const Prisoner& orig) {

}


Prisoner::~Prisoner() {

}


void Prisoner::init() {
	std::string path("data/prisoner" + std::to_string(rand() % 3) + ".png");
	img_ = MOMOS::SpriteFromFile(path.c_str());
}


void Prisoner::update(double accumTime) {
	if (aliveStatus_ == kDead)
		return;
	
	mind_->update(accumTime);
	body_->update(accumTime);
}


void Prisoner::render() {
	Agent::render();

	//Draw status indicator
	std::string status_name;
	std::string status_names[] = { "IDLE", "GONNA WORK", "WORKING LOADED", "WORKING EMPTY", "GONNA REST", "RESTING", "ESCAPING" };
	status_name = status_names[static_cast<int>(mind_->status_)];
	MOMOS::DrawText(body_->pos_.x, body_->pos_.y + 65.0f, status_name.c_str());
}


/*****************************/
/************ MIND ***********/
/*****************************/


void PrisonerMind::update(double accumTime) {
	this->sense();
	this->reason();
}


void PrisonerMind::sense() {

}





/// Switches over status and commands actions to the body
void PrisonerMind::reason() {
	int num;
	Prisoner* owner = static_cast<Prisoner*>(owner_);
	CostMap* map = GameStatus::get()->map;
	PrisonMap* prison = GameStatus::get()->prison;

	//Refactored here instead of added to every single state
	if (GameStatus::get()->alarm_mode_) {
		if (status_ != kEscaping)
			owner_->clearMovement();

		status_ = kEscaping;
	}

	switch (status_) {
	case kIdle:
		owner_->getBody()->stop();
		status_ = kGoingToRest;
		break;
	case kGoingToWork:
		if (GameStatus::get()->working_shift_ != owner_->working_shift_) {
			status_ = kGoingToRest;
			break;
		}

		//If movement to loading area has finished
		if (owner->goToRoom(prison->loading_area_)) {
			owner->clearMovement();

			status_ = kWorkingLoaded;
			time_end_status_ = GameStatus::get()->game_time + 5000.0;
		}

		break;
	case kWorkingLoaded:
		if (GameStatus::get()->working_shift_ != owner_->working_shift_) {
			owner_->speed_ = owner_->original_speed_;
			status_ = kGoingToRest;
			owner_->clearMovement();
			break;
		}

		//Load a crate if there's any left
		if (GameStatus::get()->first_available_crate_index < GameStatus::get()->crates_.size()) {
			if (owner->crate_ == nullptr) {
				owner->crate_ = GameStatus::get()->crates_[GameStatus::get()->first_available_crate_index];
				GameStatus::get()->first_available_crate_index++;
				owner->speed_ /= 2;
			}

			//Update crate position
			owner->crate_->pos_ = owner->getBody()->pos_;

			//Mark unloading zone as destination
			if (owner->goToRoom(prison->unloading_area_)) {
				owner->clearMovement();

				status_ = kWorkingUnloaded;
				time_end_status_ = GameStatus::get()->game_time + 5000.0;
			}
		} else {
			status_ = kGoingToRest;
		}
		break;
	case kWorkingUnloaded:
		if (owner->crate_ != nullptr) {
			owner->crate_ = nullptr;
			owner->speed_ *= 2;
		}

		if (GameStatus::get()->working_shift_ != owner_->working_shift_) {
			status_ = kGoingToRest;
			owner_->clearMovement();
			owner_->speed_ = owner_->original_speed_;
			break;
		}

		if (owner->goToRoom(prison->loading_area_)) {
			owner->clearMovement();

			status_ = kWorkingLoaded;
			time_end_status_ = GameStatus::get()->game_time + 5000.0;
		}
		break;
	case kGoingToRest:
		//If movement to loading area has finished
		if (owner->goToRoom(prison->resting_room_)) {
			owner->clearMovement();

			status_ = kResting;
			//time_end_status_ = GameStatus::get()->game_time + 5000.0;
		}
		break;
	case kResting:
		//wander around randomly while resting
		if (GameStatus::get()->working_shift_ == owner_->working_shift_) {
			owner_->clearMovement();
			status_ = kGoingToWork;
		} else {
			if (owner->goToRoom(prison->resting_room_)) {
				owner->clearMovement();
			}
		}
		break;
	case kEscaping:
		owner_->speed_ = owner_->original_speed_;

		//Stop updating if the prisoner reaches an exit
		PrisonAreaType area = GameStatus::get()->prison->getAreaTypeAt(owner_->getBody()->pos_);
		if (area == kBase) {
			owner_->aliveStatus_ = kDead;
			return;
		}

		if (!GameStatus::get()->alarm_mode_ && movement_finished_) {
			owner_->escape_route_set_ = false;
			owner_->door_route_set_ = false;
			status_ = kGoingToRest;
		}
		
		
		//If no movement path is set
		if (!owner_->getBody()->path_set_) {
			if (!owner_->door_route_set_) {
				owner_->clearMovement();
				owner_->door_route_set_ = true;
			}
			MOMOS::Vec2 dest = GameStatus::get()->map->MapToScreenCoords(GameStatus::get()->prison->doors_[owner_->current_target_door_]->getFrontalPoint(true));
			owner_->setPathTo(dest);
		} else {
			//If current path is complete
			if (owner_->moveFollowingPath()) {
				//if going to a door
				if (owner_->door_route_set_) {
					if (GameStatus::get()->prison->doors_[owner_->current_target_door_]->is_open_) {
						//If door is open
						MOMOS::Vec2 dest = { Screen::width - 100 , Screen::height - 50 };
						owner_->clearMovement();
						owner_->setPathTo(dest);
					} else {
						//If door is closed
						owner_->current_target_door_ = (owner_->current_target_door_ + 1) % 2;
						owner_->clearMovement();
						owner_->door_route_set_ = false;
					}
				}
			}
		}
		break;
	}
}

bool PrisonerMind::isActive() {
	return true;
}


bool PrisonerMind::isAlive() {
	return true;
}


/*****************************/
/************ BODY ***********/
/*****************************/


void PrisonerBody::update(double accumTime) {
	this->move(direction_, accumTime);
}