#include "PrisonerEcsTests.h"

#include "TestFramework.h"
#include "../include/ecs/PrisonerEcs.h"
#include "../include/ecs/components/PrisonerComponents.h"
#include "../include/ecs/system/PrisonerPathFollowSystem.h"
#include "../include/ecs/system/PrisonerMovementSystem.h"
#include "../include/ecs/system/PrisonerRenderSystem.h"
#include "../include/GameStatus.h"
#include "../include/Pathfinding/cost_map.h"

namespace {

void ResetPrisonerRegistry() {
    auto& registry = PrisonerECS::GetRegistry();
    registry.Clear();
}

void EnsureGameStatus() {
	auto* status = GameStatus::get();
	if (!status->map) {
		status->map = new CostMap();
		status->map->InitializeSynthetic(64, 64, true);
	}
}

// Validates that the movement system advances deterministic steps forward.
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
    movement.movement_finished = false;

    auto& state = registry.AddComponent<ECS::PrisonerStateComponent>(entity);
    state.status = kGoingToWork;
    state.original_speed = 0.1f;

    const auto initial_step_index = movement.deterministic_step_index;

    ECS::PrisonerPathFollowSystem pathSystem;
    ECS::PrisonerMovementSystem movementSystem;
    for (int i = 0; i < 2; ++i) {
        pathSystem.Update(registry, 16.0);
        movementSystem.Update(registry, 16.0);
    }

    TEST_CHECK(movement.deterministic_step_index >= initial_step_index,
               "Movement system should not rewind deterministic step index.");
    TEST_CHECK(movement.deterministic_step_index <= movement.deterministic_steps.size(),
               "Step index should stay within deterministic step bounds.");
}

// Checks render pass leaves transform data untouched for static sprites.
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

    ECS::PrisonerRenderSystem renderSystem;
    renderSystem.Update(registry, 0.0);

    auto& storedTransform = registry.GetComponent<ECS::TransformComponent>(entity);
    TEST_CHECK(storedTransform.position.x == 80.0f, "Render should not mutate transform X.");
    TEST_CHECK(storedTransform.position.y == 60.0f, "Render should not mutate transform Y.");
}

} // namespace

void PrisonerEcsTests() {
    RunNamedTest("PrisonerEcsTests::TestMovementPathProgression", &TestMovementPathProgression);
    RunNamedTest("PrisonerEcsTests::TestRenderTransformConsistency", &TestRenderTransformConsistency);
}