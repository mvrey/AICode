#include "../../include/ecs/PawnFactory.h"

#include "../../include/ecs/PawnEcs.h"
#include "../../include/ecs/components/PawnComponents.h"
#include "../../include/ecs/components/NeedsControllerComponent.h"
#include "../../include/Needs/NeedsController.h"
#include "../../include/Needs/Need.h"
#include "../../include/Needs/NeedId.h"
#include "../../include/Needs/NeedsConfig.h"
#include "../../include/GameStatus.h"
#include "../../include/config.h"
#include "../../include/Pathfinding/cost_map.h"

#include <MOMOS/sprite.h>

#include <cstdlib>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>
#include <memory>

namespace {

std::vector<std::string> g_pawn_names;
std::once_flag g_pawn_names_once;

void LoadPawnNames() {
	std::ifstream file("data/pawn_names.txt");
	if (!file.is_open()) {
		return;
	}

	std::string token;
	while (std::getline(file, token, ';')) {
		if (!token.empty()) {
			g_pawn_names.push_back(token);
		}
	}
}

std::string PickRandomPawnName() {
	std::call_once(g_pawn_names_once, LoadPawnNames);
	if (g_pawn_names.empty()) {
		return "Pawn Unknown";
	}
	return g_pawn_names[rand() % g_pawn_names.size()];
}

} // namespace

namespace PawnECS {

ECS::Entity SpawnPawn() {
	auto entity = PawnECS::CreatePawnEntity();
	auto& registry = PawnECS::GetRegistry();

	auto& transform = registry.AddComponent<ECS::TransformComponent>(entity);
	
	// Find a random walkable position
	::MOMOS::Vec2 spawn_position = { static_cast<float>(Screen::width) / 2.0f, static_cast<float>(Screen::height) / 2.0f };
	auto* status = GameStatus::get();
	if (status && status->map) {
		CostMap* map = status->map;
		int width = map->getWidth();
		int height = map->getHeight();
		
		// Try to find a walkable cell
		for (int attempts = 0; attempts < 1024; ++attempts) {
			int x = rand() % width;
			int y = rand() % height;
			MapCell* cell = map->getCellAt(x, y);
			if (cell && cell->isWalkable()) {
				spawn_position = map->MapToScreenCoords({ static_cast<float>(x), static_cast<float>(y) });
				break;
			}
		}
	}
	
	transform.position = spawn_position;
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

	auto& state = registry.AddComponent<ECS::PawnStateComponent>(entity);
	state.status = kGoingToWork;
	state.time_end_status = 0.0;
	state.original_speed = base_speed;
	state.name = PickRandomPawnName();

	// Initialize needs using the new NeedsController system
	auto& needs_controller_comp = registry.AddComponent<ECS::NeedsControllerComponent>(entity);
	needs_controller_comp.controller = std::make_unique<NeedsController>();

	// Load needs config
	NeedsConfig& needs_config = NeedsConfig::Get();

	// Create needs from config with random decay rates
	// Range: 0.001 to 0.005 per second (needs will deplete in 20-100 seconds if not replenished)
	constexpr float kMinDecreaseRate = 0.001f;
	constexpr float kMaxDecreaseRate = 0.005f;
	constexpr float kRateRange = kMaxDecreaseRate - kMinDecreaseRate;

	// Hunger need
	NeedConfig hunger_config = needs_config.GetConfig(NeedId::Hunger);
	float hunger_decay = kMinDecreaseRate + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * kRateRange;
	needs_controller_comp.controller->AddNeed(
		std::make_unique<Need>(NeedId::Hunger, 1.0f, hunger_decay, hunger_config.threshold)
	);

	// Energy need
	NeedConfig energy_config = needs_config.GetConfig(NeedId::Energy);
	float energy_decay = kMinDecreaseRate + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * kRateRange;
	needs_controller_comp.controller->AddNeed(
		std::make_unique<Need>(NeedId::Energy, 1.0f, energy_decay, energy_config.threshold)
	);

	// Joy need
	NeedConfig joy_config = needs_config.GetConfig(NeedId::Joy);
	float joy_decay = kMinDecreaseRate + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * kRateRange;
	needs_controller_comp.controller->AddNeed(
		std::make_unique<Need>(NeedId::Joy, 1.0f, joy_decay, joy_config.threshold)
	);

	// Also keep NeedsComponent for backward compatibility with UI
	auto& needs = registry.AddComponent<ECS::NeedsComponent>(entity);
	needs.hunger = 1.0f;
	needs.energy = 1.0f;
	needs.joy = 1.0f;
	needs.hunger_decrease_rate = hunger_decay;
	needs.energy_decrease_rate = energy_decay;
	needs.joy_decrease_rate = joy_decay;

	return entity;
}

} // namespace PawnECS

