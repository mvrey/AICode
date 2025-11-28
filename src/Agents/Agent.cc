/* 
 * File:   Agent.cc
 * Author: Marcos Vazquez
 *
 * Created on November 29, 2016, 6:35 PM
 */

#include "../../include/Agents/Agent.h"
#include "../../include/Camera.h"
#include "../../include/Agents/Pathfinder.h"

std::vector<Agent*> Agent::agents_ = {};
unsigned int Agent::last_id_ = 0;
int Agent::last_updated_id_ = -1;

Agent::Agent() {
  body_ = new AgentBody();
  mind_ = new AgentMind();
  aliveStatus_ = kAlive;
  id_ = last_id_;
  last_id_++;

  mind_->owner_ = this;
  body_->owner_ = this;

  movement_path_ = new Path();
  path_cmd_ = nullptr;
}

Agent::Agent(const Agent& orig) {
}

Agent::~Agent() {
}

void Agent::update(double accumTime) {
  if (aliveStatus_ == kDead)
    return;

  mind_->update(accumTime);
  getBody()->update(accumTime);
}


void AgentBody::update(double accumTime) {
  this->act();
}


void AgentMind::update(double accumTime) {
    this->sense();
    this->reason();
}


void AgentMind::sense() {
}


void AgentMind::reason() {

}


void AgentBody::act() {
}

void AgentBody::stop() {
	direction_ = { 0.0f, 0.0f };
}

void Agent::render() {
	float zoom = Camera::Zoom();
	::MOMOS::Vec2 screen_position = Camera::WorldToScreen(getBody()->pos_);

	float half_width = ::MOMOS::SpriteWidth(getImg()) * 0.5f * zoom;
	float half_height = ::MOMOS::SpriteHeight(getImg()) * 0.5f * zoom;

	::MOMOS::SpriteTransform transform{};
	transform.x = screen_position.x - half_width;
	transform.y = screen_position.y - half_height;
	transform.scale_x = zoom;
	transform.scale_y = zoom;

	::MOMOS::DrawSprite(getImg(), transform);
}

void AgentMind::sendMessage(Message m) {
  m.receiver->mind_->receiveMessage(m);
}

void AgentMind::receiveMessage(Message m) {
  messages_.push_back(m);
}

void AgentMind::respondMessage(Message m, bool response) {
  m.res = (response) ? kYes : kNo;
  sendMessage(m);
}



/************************************************************************/
/*							MOVEMENT METHODS                            */
/************************************************************************/



/// Moves along a vector of points
void Agent::moveDeterministic() {

	last_movement_update_ = GameStatus::get()->game_time;
	//Go from the current position to A, then to B, back to A and so on
	::MOMOS::Vec2 dest = deterministic_steps_[deterministic_step_num_];
	float dist = getBody()->getDistanceTo(dest);

	//Mark the next point as destination if we're very close to the target
	if (dist < 5.0f) {
		if (deterministic_step_num_+1 == deterministic_steps_.size()) {
			mind_->movement_finished_ = true;
		} else {
			deterministic_step_num_ = (deterministic_step_num_ + 1) % deterministic_steps_.size();
			dest = deterministic_steps_[deterministic_step_num_];
			dist = getBody()->getDistanceTo(dest);
		}
	}

	::MOMOS::Vec2 normalized = { (dest.x - getBody()->pos_.x) / dist, (dest.y - getBody()->pos_.y) / dist };
	getBody()->direction_.x = normalized.x;
	getBody()->direction_.y = normalized.y;
}


/// Selects random directions each N units of time
void Agent::moveRandom() {
	int sign;

if (GameStatus::get()->game_time - last_movement_update_ > movement_threshold_) {
	last_movement_update_ = GameStatus::get()->game_time;

	sign = rand() & 0x80000 ? 1 : -1;
	getBody()->direction_.x = ((float)(rand() % 100) / 100.0f) * sign;
	//Generate new sign for vertical movement
	sign = rand() & 0x80000 ? 1 : -1;
	getBody()->direction_.y = (1 - abs(getBody()->direction_.x)) * sign;

	//Generate new threshold
	movement_threshold_ = (double)(rand() % 1000);
}
}


/// Updates and follows the mouse cursor each N units of time.
void Agent::moveTracking() {

	if (GameStatus::get()->game_time - last_movement_update_ > movement_threshold_) {
		last_movement_update_ = GameStatus::get()->game_time;

		float mx = target_->getBody()->pos_.x;
		float my = target_->getBody()->pos_.y;
		::MOMOS::Vec2 m = { mx, my };

		float dist = getBody()->getDistanceTo(m);
		::MOMOS::Vec2 normalized = { (mx - getBody()->pos_.x) / dist, (my - getBody()->pos_.y) / dist };

		getBody()->direction_.x = normalized.x;
		getBody()->direction_.y = normalized.y;
	}
}


/// Moves the agent through a predefined number of steps
void Agent::movePattern() {

	if (GameStatus::get()->game_time - last_movement_update_ > movement_threshold_) {
		last_movement_update_ = GameStatus::get()->game_time;

		switch (pattern_steps_[step_num_]) {
		case kGoUp:
			getBody()->direction_ = { 0.0f, -1.0f };
			break;
		case kGoDown:
			getBody()->direction_ = { 0.0f, 1.0f };
			break;
		case kGoLeft:
			getBody()->direction_ = { -1.0f, 0.0f };
			break;
		case kGoRight:
			getBody()->direction_ = { 1.0f, 0.0f };
			break;
		case kStop:
			getBody()->direction_ = { 0.0f, 0.0f };
			break;
		}

		step_num_ = (step_num_ + 1) % pattern_steps_.size();
	}
}


void AgentBody::setDirection(MOMOS::Vec2 pos) {
	MOMOS::Vec2 p;
	MOMOS::Vec2 op;
	MOMOS::Vec2 dir;
	float dist;

	//Calculate vector director to target
	p = this->pos_;
	op = pos;
	dir = { (op.x - p.x), (op.y - p.y) };
	dist = sqrt((dir.x * dir.x) + (dir.y * dir.y));
	dir = { dir.x / dist, dir.y / dist };

	direction_ = dir;
}


void AgentBody::move(MOMOS::Vec2 dir, double accumTime) {
	//Movement is expressed in pixels/milisecond * num miliseconds
	pos_.x += dir.x * owner_->speed_ * (float)accumTime;
	pos_.y += dir.y * owner_->speed_ * (float)accumTime;
}


float AgentBody::getDistanceTo(MOMOS::Vec2 dest) {
	MOMOS::Vec2 p = pos_;
	return sqrt(pow(p.x - dest.x, 2) + pow(p.y - dest.y, 2));
}


void Agent::setPathTo(MOMOS::Vec2 dest) {
	Agent* owner = this;
	CostMap* map = GameStatus::get()->map;
	MOMOS::Vec2 current_map_pos = map->ScreenToMapCoords(getBody()->pos_);
	bool displaced = false;

	//Check if current cell is walkable
	while (!GameStatus::get()->map->getCellAt(current_map_pos.x, current_map_pos.y)->isWalkable()) {
		//If that's the case, reposition the agent
		if (dest.x > current_map_pos.x) current_map_pos.x++;
		if (dest.y > current_map_pos.y) current_map_pos.y++;
		displaced = true;
	}
	if (displaced) {
		getBody()->pos_ = current_map_pos;
	}

	if (!getBody()->path_set_) {

		if (path_cmd_ == nullptr) {
			path_cmd_ = new PathCommand();
			path_cmd_->start = current_map_pos;
			path_cmd_->end = map->ScreenToMapCoords(dest);
			path_cmd_->calculated = false;
			path_cmd_->pending_ = true;
			GameStatus::get()->pathfinder_->search(path_cmd_);
		}

		if (path_cmd_->pending_)
			return;

		if (path_cmd_->calculated) {
			owner->movement_path_ = path_cmd_->path_;
			delete path_cmd_;
			path_cmd_ = nullptr;
		}
		

		//Ignore unreachable paths
		if (owner->movement_path_->path_.size() > 1) {
			owner->getBody()->path_set_ = true;

			//Move deterministic
			for (unsigned int i = 0; i < owner->movement_path_->path_.size(); i++) {
				owner->deterministic_steps_.push_back(GameStatus::get()->map->MapToScreenCoords(owner->movement_path_->path_[i]));
			}
			owner->deterministic_step_num_ = 0;
		}
	}
}


bool Agent::moveFollowingPath() {
	//check if the next step is a currently walkable point
	if (!mind_->movement_finished_ && getBody()->path_set_) {
		::MOMOS::Vec2 cell_coords = GameStatus::get()->map->ScreenToMapCoords(deterministic_steps_[deterministic_step_num_]);
		MapCell* cell = GameStatus::get()->map->getCellAt((int)cell_coords.x, (int)cell_coords.y);
		if (cell->isWalkable()) {
			moveDeterministic();
			return false;
		} else {
			clearMovement();
		}
	}
	
	return true;
}


void Agent::clearMovement() {
	deterministic_steps_.clear();
	deterministic_step_num_ = 0;
	getBody()->path_set_ = false;
	getBody()->stop();
	mind_->movement_finished_ = false;
	last_movement_update_ = 0;
}