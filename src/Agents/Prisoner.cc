#include "../../include/Agents/Prisoner.h"
#include "../../include/ecs/PrisonerEcs.h"
#include <MOMOS/sprite.h>

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

	speed_ = 0.1f * GameStatus::get()->simulation_speed_;

	pattern_steps_ = { kGoLeft, kGoDown, kGoUp, kGoRight, kStop };

	init();
	InitializeEcsComponents();
}


Prisoner::Prisoner(const Prisoner& orig) {

}


Prisoner::~Prisoner() {
	if (ecs_entity_.IsValid()) {
		PrisonerECS::DestroyPrisonerEntity(ecs_entity_);
	}
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
	SyncEcsComponentsFromLegacy();
}


void Prisoner::render() {
	Agent::render();

	//Draw status indicator
	std::string status_name;
	std::string status_names[] = { "IDLE", "GONNA WORK", "WORKING LOADED", "WORKING EMPTY", "GONNA REST", "RESTING", "ESCAPING" };
	status_name = status_names[static_cast<int>(mind_->status_)];
	MOMOS::DrawText(body_->pos_.x, body_->pos_.y + 65.0f, status_name.c_str());
}

short Prisoner::GetWorkingShift() const {
	return GetStateComponent().working_shift;
}

void Prisoner::SetWorkingShift(short shift) {
	GetStateComponent().working_shift = shift;
}

ECS::PrisonerStateComponent& Prisoner::GetStateComponent() {
	return PrisonerECS::GetRegistry().GetComponent<ECS::PrisonerStateComponent>(ecs_entity_);
}

const ECS::PrisonerStateComponent& Prisoner::GetStateComponent() const {
	return PrisonerECS::GetRegistry().GetComponent<ECS::PrisonerStateComponent>(ecs_entity_);
}

ECS::MovementComponent& Prisoner::GetMovementComponent() {
	return PrisonerECS::GetRegistry().GetComponent<ECS::MovementComponent>(ecs_entity_);
}

ECS::TransformComponent& Prisoner::GetTransformComponent() {
	return PrisonerECS::GetRegistry().GetComponent<ECS::TransformComponent>(ecs_entity_);
}

ECS::SpriteComponent& Prisoner::GetSpriteComponent() {
	return PrisonerECS::GetRegistry().GetComponent<ECS::SpriteComponent>(ecs_entity_);
}

void Prisoner::InitializeEcsComponents() {
	ecs_entity_ = PrisonerECS::CreatePrisonerEntity();
	auto& registry = PrisonerECS::GetRegistry();

	auto& transform = registry.AddComponent<ECS::TransformComponent>(ecs_entity_);
	transform.position = body_->pos_;
	transform.direction = body_->direction_;

	auto& sprite = registry.AddComponent<ECS::SpriteComponent>(ecs_entity_);
	sprite.sprite = img_;
	sprite.width = img_ ? static_cast<float>(MOMOS::SpriteWidth(img_)) : 0.0f;
	sprite.height = img_ ? static_cast<float>(MOMOS::SpriteHeight(img_)) : 0.0f;

	auto& movement = registry.AddComponent<ECS::MovementComponent>(ecs_entity_);
	movement.speed = speed_;
	movement.last_movement_update = last_movement_update_;
	movement.movement_threshold = movement_threshold_;
	movement.deterministic_steps = deterministic_steps_;
	movement.deterministic_step_index = deterministic_step_num_;
	movement.path_set = getBody()->path_set_;
	movement.movement_path = movement_path_;
	movement.path_command = path_cmd_;

	auto& state = registry.AddComponent<ECS::PrisonerStateComponent>(ecs_entity_);
	state.status = mind_->status_;
	state.time_end_status = mind_->time_end_status_;
	state.working_shift = 0;
	state.current_target_door = 1;
	state.original_speed = speed_;
}

void Prisoner::SyncEcsComponentsFromLegacy() {
	auto& registry = PrisonerECS::GetRegistry();

	auto& transform = registry.GetComponent<ECS::TransformComponent>(ecs_entity_);
	transform.position = body_->pos_;
	transform.direction = body_->direction_;

	auto& sprite = registry.GetComponent<ECS::SpriteComponent>(ecs_entity_);
	sprite.sprite = img_;
	sprite.width = img_ ? static_cast<float>(MOMOS::SpriteWidth(img_)) : 0.0f;
	sprite.height = img_ ? static_cast<float>(MOMOS::SpriteHeight(img_)) : 0.0f;

	auto& movement = registry.GetComponent<ECS::MovementComponent>(ecs_entity_);
	movement.speed = speed_;
	movement.last_movement_update = last_movement_update_;
	movement.movement_threshold = movement_threshold_;
	movement.deterministic_steps = deterministic_steps_;
	movement.deterministic_step_index = deterministic_step_num_;
	movement.path_set = getBody()->path_set_;
	movement.movement_path = movement_path_;
	movement.path_command = path_cmd_;

	auto& state = registry.GetComponent<ECS::PrisonerStateComponent>(ecs_entity_);
	state.status = mind_->status_;
	state.time_end_status = mind_->time_end_status_;
	state.pursuit_target = mind_->target_;
	state.original_speed = (state.original_speed == 0.0f) ? speed_ : state.original_speed;
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
	auto& state = owner->GetStateComponent();

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
		if (GameStatus::get()->working_shift_ != state.working_shift) {
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
		if (GameStatus::get()->working_shift_ != state.working_shift) {
			owner_->speed_ = state.original_speed;
			status_ = kGoingToRest;
			owner_->clearMovement();
			break;
		}

		//Load a crate if there's any left
		if (GameStatus::get()->first_available_crate_index < GameStatus::get()->crates_.size()) {
			if (state.carried_crate == nullptr) {
				state.carried_crate = GameStatus::get()->crates_[GameStatus::get()->first_available_crate_index];
				GameStatus::get()->first_available_crate_index++;
				owner->speed_ /= 2;
			}

			//Update crate position
			state.carried_crate->pos_ = owner->getBody()->pos_;

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
		if (state.carried_crate != nullptr) {
			state.carried_crate = nullptr;
			owner->speed_ *= 2;
		}

		if (GameStatus::get()->working_shift_ != state.working_shift) {
			status_ = kGoingToRest;
			owner_->clearMovement();
			owner_->speed_ = state.original_speed;
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
		if (GameStatus::get()->working_shift_ == state.working_shift) {
			owner_->clearMovement();
			status_ = kGoingToWork;
		} else {
			if (owner->goToRoom(prison->resting_room_)) {
				owner->clearMovement();
			}
		}
		break;
	case kEscaping:
		owner_->speed_ = state.original_speed;

		//Stop updating if the prisoner reaches an exit
		PrisonAreaType area = GameStatus::get()->prison->getAreaTypeAt(owner_->getBody()->pos_);
		if (area == kBase) {
			owner_->aliveStatus_ = kDead;
			return;
		}

		if (!GameStatus::get()->alarm_mode_ && movement_finished_) {
			state.escape_route_set = false;
			state.door_route_set = false;
			status_ = kGoingToRest;
		}
		
		
		//If no movement path is set
		if (!owner_->getBody()->path_set_) {
			if (!state.door_route_set) {
				owner_->clearMovement();
				state.door_route_set = true;
			}
			MOMOS::Vec2 dest = GameStatus::get()->map->MapToScreenCoords(GameStatus::get()->prison->doors_[state.current_target_door]->getFrontalPoint(true));
			owner_->setPathTo(dest);
		} else {
			//If current path is complete
			if (owner_->moveFollowingPath()) {
				//if going to a door
				if (state.door_route_set) {
					if (GameStatus::get()->prison->doors_[state.current_target_door]->is_open_) {
						//If door is open
						MOMOS::Vec2 dest = { Screen::width - 100 , Screen::height - 50 };
						owner_->clearMovement();
						owner_->setPathTo(dest);
					} else {
						//If door is closed
						state.current_target_door = (state.current_target_door + 1) % 2;
						owner_->clearMovement();
						state.door_route_set = false;
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