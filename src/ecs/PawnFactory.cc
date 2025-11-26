#include "../../include/ecs/PawnFactory.h"

#include "../../include/ecs/PawnEcs.h"
#include "../../include/ecs/components/PawnComponents.h"
#include "../../include/GameStatus.h"
#include "../../include/config.h"

#include <MOMOS/sprite.h>

#include <cstdlib>
#include <string>

namespace PawnECS {

ECS::Entity SpawnPawn() {
	auto entity = PawnECS::CreatePawnEntity();
	auto& registry = PawnECS::GetRegistry();

	auto& transform = registry.AddComponent<ECS::TransformComponent>(entity);
	transform.position = { static_cast<float>(Screen::width) / 2.0f, static_cast<float>(Screen::height) / 2.0f };
	transform.direction = { -1.0f, 0.0f };
	transform.rotation = 0.0f;

	auto& sprite = registry.AddComponent<ECS::SpriteComponent>(entity);
	std::string path = "data/pawn" + std::to_string(rand() % 3) + ".png";
	sprite.sprite = MOMOS::SpriteFromFile(path.c_str());
	sprite.width = sprite.sprite ? static_cast<float>(MOMOS::SpriteWidth(sprite.sprite)) : 0.0f;
	sprite.height = sprite.sprite ? static_cast<float>(MOMOS::SpriteHeight(sprite.sprite)) : 0.0f;

	auto& movement = registry.AddComponent<ECS::MovementComponent>(entity);
	constexpr float kBasePAWNSpeed = 0.05f;
	float base_speed = kBasePAWNSpeed * GameStatus::get()->simulation_speed_;
	movement.speed = base_speed;
	movement.last_movement_update = 0.0;
	movement.movement_threshold = 3000.0;
	movement.deterministic_steps.clear();
	movement.deterministic_step_index = 0;
	movement.path_set = false;
	movement.path_command = nullptr;
	movement.movement_path = nullptr;
	movement.movement_finished = false;

	auto& state = registry.AddComponent<ECS::PAWNStateComponent>(entity);
	state.status = kGoingToWork;
	state.time_end_status = 0.0;
	state.original_speed = base_speed;

	return entity;
}

} // namespace PawnECS

