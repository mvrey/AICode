#include "../../include/Agents/Soldier.h"

/*****************************/
/************ AGENT **********/
/*****************************/


Soldier::Soldier() {
	body_ = new SoldierBody();
	mind_ = new SoldierMind();
	body_->owner_ = this;
	mind_->owner_ = this;

	body_->pos_ = { Screen::width / 3, Screen::height / 3 };
	body_->direction_ = { -1.0f, 0.0f };

	last_movement_update_ = 0.0f;
	movement_threshold_ = 3000.0;

	speed_ = 0.1f * GameStatus::get()->simulation_speed_;

	pattern_steps_ = { kGoLeft, kGoDown, kGoUp, kGoRight, kStop };
	movement_path_ = new Path();

	init();
}


Soldier::Soldier(const Soldier& orig) {

}


Soldier::~Soldier() {

}


void Soldier::init() {
	std::string path("data/agent" + std::to_string(rand() % 1) + ".png");
	img_ = MOMOS::SpriteFromFile(path.c_str());
}


void Soldier::update(double accumTime) {
	if (true) {
		mind_->update(accumTime);
		body_->update(accumTime);
	}
}


void Soldier::render() {
	Agent::render();
}

/*****************************/
/************ MIND ***********/
/*****************************/


void SoldierMind::update(double accumTime) {
	this->sense();
	this->reason();
}


void SoldierMind::sense() {
	//Search for doors within detection radius
	for (unsigned int i = 0; i < GameStatus::get()->prison->doors_.size(); i++) {
		Door* door = GameStatus::get()->prison->doors_[i];
		if (owner_->getBody()->getDistanceTo(GameStatus::get()->map->MapToScreenCoords(door->pos_)) < detection_radius_) {
			door->open();
			door->locked_open_time_ = GameStatus::get()->game_time + 500;
		}
	}
}


/// Switches over status and commands actions to the body
void SoldierMind::reason() {
	PrisonAreaType a = GameStatus::get()->prison->getAreaTypeAt(GameStatus::get()->map->ScreenToMapCoords(owner_->getBody()->pos_));
	bool inside = (a==kBuilding || a==kCourtyard || a==kYard);
	MOMOS::Vec2 base_pos = { Screen::width - 100 , Screen::height - 50 };
	PrisonAreaType area;

	if (GameStatus::get()->alarm_mode_) {
		status_ = kRunning;
	}

	switch (status_) {
	case kStandard:
		if (!owner_->getBody()->path_set_) {
			//Go to the front of the door
			owner_->clearMovement();
			MOMOS::Vec2 point = GameStatus::get()->prison->doors_[owner_->current_target_door_]->getFrontalPoint(inside);
			owner_->setPathTo(GameStatus::get()->map->MapToScreenCoords(point));
		} else {
			if (owner_->moveFollowingPath()) {
				owner_->clearMovement();
				//If destination reached, mark next door as destination
				owner_->current_target_door_ = (owner_->current_target_door_ + 1) % 2;
			}
		}
		break;
	case kRunning:
		//Stop updating if the soldier reaches an exit
		area = GameStatus::get()->prison->getAreaTypeAt(owner_->getBody()->pos_);
		if (area == kBase) {
			owner_->aliveStatus_ = kDead;
			return;
		}

		if (!GameStatus::get()->alarm_mode_ && movement_finished_) {
			status_ = kStandard;
			owner_->escape_route_set_ = false;
		}

		if (!owner_->escaped) {
			if (owner_->moveFollowingPath()) {
				owner_->clearMovement();

				owner_->setPathTo(base_pos);
				owner_->escape_route_set_ = true;
			}
		}

		break;
	default:
		break;
	}
}


/*****************************/
/************ BODY ***********/
/*****************************/


void SoldierBody::update(double accumTime) {
	this->move(direction_, accumTime);
}