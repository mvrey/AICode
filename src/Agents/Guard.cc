#include "../../include/Agents/Guard.h"
#include "../../include/Agents/Soldier.h"
#include "../../include/Agents/Prisoner.h"

/*****************************/
/************ AGENT **********/
/*****************************/


Guard::Guard() {
	body_ = new GuardBody();
	mind_ = new GuardMind();
	body_->owner_ = this;
	mind_->owner_ = this;
	
	body_->size_ = 31;

	body_->pos_ = { Screen::width / 2, Screen::height / 2 };
	body_->direction_ = { -1.0f, 0.0f };

	last_movement_update_ = 0.0f;
	movement_threshold_ = 500.0;
	mind_->status_ = kNormal;

	speed_ = 0.1f * GameStatus::get()->simulation_speed_;

	pattern_steps_ = { kGoLeft, kGoDown, kGoUp, kGoRight, kStop };

	init();
}


Guard::Guard(const Guard& orig) {

}


Guard::~Guard() {

}


void Guard::init() {
	std::string path("data/guard" + std::to_string(rand()%4) + ".png");
	img_ = MOMOS::SpriteFromFile(path.c_str());
}


void Guard::update(double accumTime) {
	if (true) {
		mind_->update(accumTime);
		body_->update(accumTime);
	}
}


void Guard::render() {
	Agent::render();

	//Draw status indicator
	std::string status_name;
	std::string status_names[] = { "NORMAL", "SUSPICIOUS", "ALERT" };
	status_name = status_names[static_cast<int>(mind_->status_)];
	MOMOS::DrawText(body_->pos_.x, body_->pos_.y + 65.0f, status_name.c_str());

	//Draw vision cone
	renderVision();
}





/*****************************/
/************ MIND ***********/
/*****************************/


void GuardMind::update(double accumTime) {
	this->sense();
	this->reason();
}


void GuardMind::sense() {

	//Get vision cone geometry
	owner_->calculateVision();

	//Reset sensorization flags
	door_spotted_ = false;
	soldier_spotted_ = false;

	//Search for open doors within detection radius and close them
	for (unsigned int i = 0; i < GameStatus::get()->prison->doors_.size(); i++) {
		Door* door = GameStatus::get()->prison->doors_[i];
		if (door->is_open_ && owner_->getBody()->getDistanceTo(GameStatus::get()->map->MapToScreenCoords(door->pos_)) < detection_radius_) {
			door->close();
			owner_->clearMovement();

			//Set alarm mode
			GameStatus::get()->alarm_mode_ = true;
			GameStatus::get()->alarm_mode_time_end_ = GameStatus::get()->game_time + 10000.0;
			return;
		}
	}


	//Search for open doors within vision cone
	for (unsigned int i = 0; i < GameStatus::get()->prison->doors_.size(); i++) {
		Door* door = GameStatus::get()->prison->doors_[i];
		door_pos_ = GameStatus::get()->map->MapToScreenCoords(door->pos_);
		if (door->is_open_ && owner_->isPointSighted(door_pos_)) {
			door_spotted_ = true;
		}
	}


	//Search for soldiers within vision cone
	for(unsigned int i = 0; i < GameStatus::get()->soldiers_.size(); i++) {
		Soldier* soldier = GameStatus::get()->soldiers_[i];

		if (owner_->isPointSighted(soldier->getBody()->pos_)) {
			soldier_spotted_ = true;
			if (status_ == kAlert) {
				GameStatus::get()->alarm_mode_time_end_ = GameStatus::get()->game_time + 10000.0;
				time_end_status_ = GameStatus::get()->alarm_mode_time_end_;
			}

			owner_->clearMovement();
			owner_->target_ = soldier;
		}
	}

	//On alarm mode, also chase prisoners
	if (status_ == kAlert) {
		for (unsigned int i = 0; i < GameStatus::get()->prisoners_.size(); i++) {
			Prisoner* prisoner = GameStatus::get()->prisoners_[i];

			if (owner_->isPointSighted(prisoner->getBody()->pos_)) {

				time_end_status_ = GameStatus::get()->alarm_mode_time_end_;

				owner_->clearMovement();
				owner_->target_ = prisoner;
			}
		}
	}
}


/// Switches over status and commands actions to the body
void GuardMind::reason() {
	int num;

	//Reset status timer to alarm time
	if (GameStatus::get()->alarm_mode_) {
		status_ = kAlert;
		time_end_status_ = GameStatus::get()->alarm_mode_time_end_;
	}

	//This is refactored here instead of added to every other state
	if (door_spotted_ && status_ != kAlert) {
		status_ = kAlert;
		time_end_status_ = GameStatus::get()->game_time + 10000.0;
		//Advance towards the open door
		owner_->clearMovement();
		owner_->setPathTo(door_pos_);
	}

	//Same refactoring as above
	if (soldier_spotted_ && status_ != kAlert) {
		status_ = kSuspect;
		time_end_status_ = GameStatus::get()->game_time + 5000.0;
	}

	switch (status_) {
	case kNormal:
		if (!owner_->getBody()->path_set_) {
			//Pick random point in prison
			MOMOS::Vec2 point = { (float)(rand() % Screen::width),(float)( rand() % Screen::height) };

			//Check area type
			Room* room = GameStatus::get()->prison->getRoomAt(GameStatus::get()->map->ScreenToMapCoords(point));
			PrisonAreaType at = GameStatus::get()->prison->getAreaTypeAt(point);

			if (room != nullptr) {
				owner_->goToRoom(*room);
			}

			if (!owner_->getBody()->path_set_ && at == kBuilding || at == kCourtyard) {
				owner_->setPathTo(point);
			}
		} else {
			if (owner_->moveFollowingPath()) {
				owner_->clearMovement();
			}
		}
		break;
	case kSuspect:
		if (GameStatus::get()->game_time < time_end_status_ && owner_->target_->aliveStatus_ == kAlive) {
			owner_->moveTracking();
		} else {
			status_ = kNormal;
			owner_->target_ = nullptr;
		}
		break;
	case kAlert:

		if (GameStatus::get()->game_time < time_end_status_) {
			if (owner_->target_ != nullptr) {
				//Chase target prisoner/soldier
				if (owner_->target_->aliveStatus_ == kAlive)
					owner_->moveTracking();
				else {
					owner_->clearMovement();
					owner_->target_ = nullptr;
				}
			//If no enemy on sight, keep patrolling
			} else if (owner_->getBody()->path_set_) {
				if (owner_->moveFollowingPath()) {
					owner_->clearMovement();
				}
			} else {
				//Pick random point in prison
				MOMOS::Vec2 point = { (float)(rand() % Screen::width), (float)(rand() % Screen::height) };

				//Check area type
				Room* room = GameStatus::get()->prison->getRoomAt(GameStatus::get()->map->ScreenToMapCoords(point));
				PrisonAreaType at = GameStatus::get()->prison->getAreaTypeAt(point);

				if (room != nullptr) {
					owner_->goToRoom(*room);
				}

				if (!owner_->getBody()->path_set_ && at == kBuilding || at == kCourtyard) {
					owner_->setPathTo(point);
				}
			}
		} else {
			status_ = kNormal;
			owner_->target_ = nullptr;
		}
		break;
	}
}


bool GuardMind::isActive() {
	return true;
}


bool GuardMind::isAlive() {
	return true;
}


/*****************************/
/************ BODY ***********/
/*****************************/


void GuardBody::update(double accumTime) {
	this->move(direction_, accumTime);
}


/*****************************/
/************ SIGHT **********/
/*****************************/

float Guard::getVisionSign(MOMOS::Vec2 p1, MOMOS::Vec2 p2, MOMOS::Vec2 p3) {
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}


bool Guard::isPointSighted(MOMOS::Vec2 pt) {

	int i, j = 0;
	bool c = false;

	//Raycast vision cone searching for an even number of line intersections
	for (i = 0, j = (int)vision_cone_amplitude_ - 1; i < (int)vision_cone_amplitude_; j = i++) {
		if (((vision_cone_points_[i].y > pt.y) != (vision_cone_points_[j].y > pt.y)) &&
			(pt.x < (vision_cone_points_[j].x - vision_cone_points_[i].x) * (pt.y - vision_cone_points_[i].y) / (vision_cone_points_[j].y - vision_cone_points_[i].y) + vision_cone_points_[i].x))
			c = !c;
	}
	return c;
}


void Guard::calculateVision() {

	float vision_cone_pathpoints[1000];
	vision_cone_pathpoints[0] = 0.0f;
	vision_cone_pathpoints[1] = 0.0f;
	vision_cone_pathpoints[2] = vision_cone_length_;
	vision_cone_pathpoints[3] = -vision_cone_amplitude_ / 2;
	for (int i = 0; i < (int)vision_cone_amplitude_; i++) {
		vision_cone_pathpoints[2 * i + 4] = vision_cone_length_;
		vision_cone_pathpoints[2 * i + 5] = (-vision_cone_amplitude_ / 2) + i;
	}
	vision_cone_pathpoints[(int)vision_cone_amplitude_ * 2] = 0.0f;
	vision_cone_pathpoints[(int)vision_cone_amplitude_ * 2 + 1] = 0.0f;


	MOMOS::Mat3 trans = MOMOS::Mat3Translate(body_->pos_.x, body_->pos_.y);
	float radians = atan2(getBody()->direction_.y, getBody()->direction_.x);
	MOMOS::Mat3 rot = MOMOS::Mat3Rotate(radians);
	MOMOS::Mat3 transform = MOMOS::Mat3Multiply(trans, rot);

	for (int i = 0; i <= (int)vision_cone_amplitude_; i ++) {
		MOMOS::Vec2 point = { vision_cone_pathpoints[i * 2], vision_cone_pathpoints[i * 2 + 1] };
		MOMOS::Vec2 transformed_point = MOMOS::Mat3TransformVec2(transform, point);
		vision_cone_translated_points_[i * 2 + 0] = transformed_point.x;
		vision_cone_translated_points_[i * 2 + 1] = transformed_point.y;
		vision_cone_points_[i] = { transformed_point.x, transformed_point.y };
	}

	removeXRayPowers();
}


void Guard::removeXRayPowers() {
	//For every point (segment) in the vision cone, raytrace from vision origin
	for(int k=0; k < vision_cone_amplitude_; k++) {
		//Get common starting point
		MOMOS::Vec2 p = { vision_cone_translated_points_[0], vision_cone_translated_points_[1] };
		MOMOS::Vec2 start = GameStatus::get()->map->ScreenToMapCoords(MOMOS::Vec2(p));
	
		//Get cone base point (raycast end point)
		p = { vision_cone_translated_points_[k * 2 + 2], vision_cone_translated_points_[k * 2 + 3] };
		MOMOS::Vec2 end = GameStatus::get()->map->ScreenToMapCoords(MOMOS::Vec2(p));
		
		//Calculate axis steps
		MOMOS::Vec2 current_end_point = end;

		float dx = (end.x - start.x);
		float dy = abs(end.y - start.y);
		float step_x, step_y = 0;

		if (dy == 0) {
			step_x = (start.x < current_end_point.x) ? 1.0f : -1.0f;
			step_y = 0.0f;
		} else {
			step_x = dx / dy;
			if (start.x < end.x) step_x = fabs(step_x);

			step_y = (start.y < current_end_point.y) ? 1.0f : -1.0f;

			//Apply step fine tunning
			if (fabs(step_x) > 1.0f) {
				step_y /= fabs(step_x);
				step_x = (start.x < current_end_point.x) ? 1.0f : -1.0f;
			}
		}

		//Advance from raycast start to end points
		bool exit = false;
		while ( (round(start.x) != round(current_end_point.x) || round(start.y) != round(current_end_point.y)) && !exit) {
				//Raytrace from origin to "vertical" destination point
				MOMOS::Vec2 current_point = { start.x, start.y };

				//If at any point of the trace we find unwalkable terrain, stop there
				if (GameStatus::get()->map->isWalkable(current_point) == false) {
					//Alter vision cone points position
					MOMOS::Vec2 scr_coords = GameStatus::get()->map->MapToScreenCoords(current_point);
					vision_cone_translated_points_[k * 2 + 2] = scr_coords.x;
					vision_cone_translated_points_[k * 2 + 3] = scr_coords.y;
					vision_cone_points_[k] = { scr_coords.x, scr_coords.y };
					exit = true;
				}
			//Advance raycast to next point
			start = { start.x + step_x, start.y + step_y };
		}
	}
}


void Guard::renderVision() {
	//Vision cone
	if (true) {
		
		//set cone color based on agent alert status
		unsigned char color[4];
		color[3] = 80;
		switch (mind_->status_) {
		case kSuspect:
			color[0] = 200;
			color[1] = 200;
			color[2] = 0;
			break;
		case kAlert:
			color[0] = 200;
			color[1] = 0;
			color[2] = 0;
			break;
		default:
			color[0] = 0;
			color[1] = 200;
			color[2] = 0;
			break;
		}

		MOMOS::DrawSetStrokeColor(200, 200, 200, 200);
		MOMOS::DrawSetFillColor(color[0], color[1], color[2], color[3]);
		MOMOS::DrawSolidPath(vision_cone_translated_points_, (int)vision_cone_amplitude_+1);
	}
}