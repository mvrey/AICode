
#include "../../include/Agents/Prisoner.h"
#include "../../include/ecs/PrisonerEcs.h"
#include "../../include/ecs/PrisonerMovementUtils.h"
#include <MOMOS/sprite.h>

namespace {
namespace MovementHelpers = PrisonerECS::MovementUtils;
}

/*****************************/
/************ AGENT **********/
/*****************************/

AgentBody* Prisoner::getBody() {
	auto& transform = GetTransformComponent();
	body_->pos_ = transform.position;
	body_->direction_ = transform.direction;
	return body_;
}

bool Prisoner::goToRoom(Room room) {
	auto& movement = GetMovementComponent();
	auto& transform = GetTransformComponent();

	CostMap* map = GameStatus::get()->map;
	PrisonMap* prison = GameStatus::get()->prison;

	if (!map || !prison) {
		return false;
	}

	if (!movement.path_set) {
		Room* current_room = prison->getRoomAt(map->ScreenToMapCoords(transform.position));
		std::vector<::MOMOS::Vec2> waypoint_path;

		if (current_room != nullptr) {
			if (current_room->id_ == room.id_) {
				MovementHelpers::ClearMovement(*this);
				waypoint_path.push_back(prison->getRandomPointInRoom(room));
			} else {
				waypoint_path = prison->getPathToRoom(current_room, &room);
			}
		}

		if (!waypoint_path.empty()) {
			movement.path_set = true;
			movement.deterministic_steps.clear();
			for (const auto& waypoint : waypoint_path) {
				movement.deterministic_steps.push_back(map->MapToScreenCoords(waypoint));
			}
			movement.deterministic_steps.push_back(
				map->MapToScreenCoords(prison->getRandomPointInRoom(room)));
			movement.deterministic_step_index = 0;
			movement.movement_path = nullptr;
			movement.path_command = nullptr;
			movement.movement_finished = false;
			SyncLegacyFromEcs();
		} else {
			::MOMOS::Vec2 point = map->MapToScreenCoords(prison->getRandomPointInRoom(room));
			MovementHelpers::SetPathTo(*this, point);
			SyncLegacyFromEcs();
		}

		return false;
	}

	const bool finished = MovementHelpers::MoveFollowingPath(*this);
	SyncLegacyFromEcs();
	return finished;
}

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
	
	// Legacy mind/body remain for now but rely on ECS data
	auto& registry = PrisonerECS::GetRegistry();
	auto& transform = registry.GetComponent<ECS::TransformComponent>(ecs_entity_);
	auto& sprite = registry.GetComponent<ECS::SpriteComponent>(ecs_entity_);

	body_->pos_ = transform.position;
	body_->direction_ = transform.direction;
	img_ = sprite.sprite;

	SyncLegacyFromEcs();

	mind_->update(accumTime);
	body_->update(accumTime);

	SyncEcsComponentsFromLegacy();
}


void Prisoner::render() {
	auto& registry = PrisonerECS::GetRegistry();
	if (registry.HasComponent<ECS::SpriteComponent>(ecs_entity_)) {
		auto& sprite = registry.GetComponent<ECS::SpriteComponent>(ecs_entity_);
		auto& transform = registry.GetComponent<ECS::TransformComponent>(ecs_entity_);
		if (sprite.sprite) {
			float draw_x = transform.position.x - sprite.width / 2.0f;
			float draw_y = transform.position.y - sprite.height / 2.0f;
			MOMOS::DrawSprite(sprite.sprite, draw_x, draw_y);
		}
	}

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
	movement.path_set = body_->path_set_;
	movement.movement_path = movement_path_;
	movement.path_command = path_cmd_;
	movement.movement_finished = mind_ ? mind_->movement_finished_ : false;

	auto& state = registry.AddComponent<ECS::PrisonerStateComponent>(ecs_entity_);
	state.status = mind_->status_;
	state.time_end_status = mind_->time_end_status_;
	state.working_shift = 0;
	state.original_speed = speed_;
	state.owner = this;
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
	movement.movement_finished = mind_ ? mind_->movement_finished_ : movement.movement_finished;

	auto& state = registry.GetComponent<ECS::PrisonerStateComponent>(ecs_entity_);
	state.owner = this;
	state.pursuit_target = mind_ ? mind_->target_ : nullptr;
	state.original_speed = (state.original_speed == 0.0f) ? speed_ : state.original_speed;
}

void Prisoner::SyncLegacyFromEcs() {
	auto& registry = PrisonerECS::GetRegistry();
	auto& movement = registry.GetComponent<ECS::MovementComponent>(ecs_entity_);
	auto& state = registry.GetComponent<ECS::PrisonerStateComponent>(ecs_entity_);

	speed_ = movement.speed;
	last_movement_update_ = movement.last_movement_update;
	movement_threshold_ = movement.movement_threshold;
	deterministic_steps_ = movement.deterministic_steps;
	deterministic_step_num_ = movement.deterministic_step_index;
	movement_path_ = movement.movement_path;
	path_cmd_ = movement.path_command;
	if (body_) {
		body_->path_set_ = movement.path_set;
	}
	if (mind_) {
		mind_->movement_finished_ = movement.movement_finished;
		mind_->status_ = state.status;
		mind_->time_end_status_ = state.time_end_status;
		mind_->target_ = state.pursuit_target;
	}
}


/*****************************/
/************ MIND ***********/
/*****************************/


void PrisonerMind::update(double /*accumTime*/) {
	// AI handled by ECS systems; mind remains for legacy data sync only.
}


void PrisonerMind::sense() {

}





/// Switches over status and commands actions to the body
void PrisonerMind::reason() {}



/*****************************/
/************ BODY ***********/
/*****************************/


void PrisonerBody::update(double accumTime) {
	this->move(direction_, accumTime);
}