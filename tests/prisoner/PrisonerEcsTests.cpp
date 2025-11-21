#include <cassert>
#include <vector>

#include "../../include/ecs/PrisonerEcs.h"
#include "../../include/ecs/PrisonerEcsSystems.h"
#include "../../include/ecs/PrisonerMovementUtils.h"
#include "../../include/ecs/components/PrisonerComponents.h"
#include "../../include/GameStatus.h"
#include "../../include/PrisonMap.h"
#include "../../include/Pathfinding/cost_map.h"
#include "../../include/Agents/Prisoner.h"

namespace {

void ResetPrisonerRegistry() {
	auto& registry = PrisonerECS::GetRegistry();
	registry = ECS::Registry{};
}

void EnsureGameStatus() {
	auto* status = GameStatus::get();
	if (!status->map) {
		status->map = new CostMap();
		status->map->Load("data/map_03_60x44_bw.bmp", "data/map_03_960x704_layoutAB.bmp");
	}
	if (!status->prison) {
		status->prison = new PrisonMap();
	}
}

void TestMovementPathProgression() {
	ResetPrisonerRegistry();
	EnsureGameStatus();

	auto& registry = PrisonerECS::GetRegistry();
	ECS::Entity entity = registry.CreateEntity();

	auto& transform = registry.AddComponent<ECS::TransformComponent>(entity);
	transform.position = { 100.0f, 100.0f };

	auto& sprite = registry.AddComponent<ECS::SpriteComponent>(entity);
	sprite.sprite = nullptr;

	auto& movement = registry.AddComponent<ECS::MovementComponent>(entity);
	movement.speed = 0.1f;
	movement.deterministic_steps.push_back({ 150.0f, 100.0f });
	movement.deterministic_steps.push_back({ 200.0f, 100.0f });
	movement.path_set = true;

	auto& state = registry.AddComponent<ECS::PrisonerStateComponent>(entity);
	state.status = kGoingToWork;
	state.original_speed = 0.1f;

	PrisonerECS::MovementUtils::MoveFollowingPath(*reinterpret_cast<Prisoner*>(state.owner));
	PrisonerECS::Systems::Get().Update(16.0);
	PrisonerECS::Systems::Get().Update(16.0);

	assert(movement.deterministic_step_index <= movement.deterministic_steps.size());
}

void TestEscapeStateTransitions() {
	ResetPrisonerRegistry();
	EnsureGameStatus();

	auto& registry = PrisonerECS::GetRegistry();
	ECS::Entity entity = registry.CreateEntity();

	auto& transform = registry.AddComponent<ECS::TransformComponent>(entity);
	transform.position = { 100.0f, 100.0f };

	auto& movement = registry.AddComponent<ECS::MovementComponent>(entity);
	movement.speed = 0.1f;

	auto& state = registry.AddComponent<ECS::PrisonerStateComponent>(entity);
	state.status = kEscaping;
	state.original_speed = 0.1f;

	GameStatus::get()->alarm_mode_ = true;
	PrisonerECS::Systems::Get().Update(16.0);
	assert(state.status == kEscaping);

	GameStatus::get()->alarm_mode_ = false;
	movement.movement_finished = true;
	PrisonerECS::Systems::Get().Update(16.0);
	assert(state.status == kGoingToRest);
}

void TestRenderTransformConsistency() {
	ResetPrisonerRegistry();
	auto& registry = PrisonerECS::GetRegistry();
	ECS::Entity entity = registry.CreateEntity();

	auto& transform = registry.AddComponent<ECS::TransformComponent>(entity);
	transform.position = { 80.0f, 60.0f };

	auto& sprite = registry.AddComponent<ECS::SpriteComponent>(entity);
	sprite.sprite = nullptr;
	sprite.width = 32.0f;
	sprite.height = 32.0f;

	PrisonerECS::Systems::Get().Render(0.0);

	auto& storedTransform = registry.GetComponent<ECS::TransformComponent>(entity);
	assert(storedTransform.position.x == 80.0f);
	assert(storedTransform.position.y == 60.0f);
}

} // namespace

int main() {
	TestMovementPathProgression();
	TestEscapeStateTransitions();
	TestRenderTransformConsistency();
	return 0;
}

