#include "RegistryTests.h"

#include "TestFramework.h"
#include "../include/ecs/Registry.h"

#include <cstdio>

using namespace ECS;

struct PositionComponent {
    float x = 0.0f;
    float y = 0.0f;
};

struct VelocityComponent {
    float vx = 0.0f;
    float vy = 0.0f;
};

void TestEntityReuse() {
    Registry registry;
    Entity e1 = registry.CreateEntity();
    Entity e2 = registry.CreateEntity();
    TEST_CHECK(e1 != e2, "Entities created consecutively should be unique.");

    registry.DestroyEntity(e1);
    Entity e3 = registry.CreateEntity();

    TEST_CHECK(e3 == e1, "Entity ids should be reused after destruction.");
}

void TestAddGetComponent() {
    Registry registry;
    Entity e = registry.CreateEntity();

    auto& pos = registry.AddComponent<PositionComponent>(e, PositionComponent{ 3.0f, 4.0f });
    TEST_CHECK(pos.x == 3.0f && pos.y == 4.0f, "Component should be constructed with provided values.");

    TEST_CHECK(registry.HasComponent<PositionComponent>(e), "Registry should report the component as present.");
    const auto& fetched = registry.GetComponent<PositionComponent>(e);
    TEST_CHECK(fetched.x == 3.0f && fetched.y == 4.0f, "Fetched component values should match stored ones.");

    registry.RemoveComponent<PositionComponent>(e);
    TEST_CHECK(!registry.HasComponent<PositionComponent>(e), "Component removal should clear presence flag.");
}

void TestIteration() {
    Registry registry;
    Entity e1 = registry.CreateEntity();
    Entity e2 = registry.CreateEntity();
    Entity e3 = registry.CreateEntity();

    registry.AddComponent<PositionComponent>(e1, PositionComponent{ 1.0f, 2.0f });
    registry.AddComponent<PositionComponent>(e2, PositionComponent{ 3.0f, 4.0f });
    registry.AddComponent<PositionComponent>(e3, PositionComponent{ 5.0f, 6.0f });

    int count = 0;
    registry.ForEach<PositionComponent>([&](Entity entity, PositionComponent& pos) {
        TEST_CHECK(entity.IsValid(), "Entities yielded during iteration must be valid.");
        pos.x += 1.0f;
        pos.y += 1.0f;
        ++count;
    });

    TEST_CHECK(count == 3, "ForEach should visit every matching entity.");
    TEST_CHECK(registry.GetComponent<PositionComponent>(e1).x == 2.0f, "Iteration should mutate first component.");
    TEST_CHECK(registry.GetComponent<PositionComponent>(e3).y == 7.0f, "Iteration should mutate last component.");
}

void TestDestroyRemovesComponents() {
    Registry registry;
    Entity e = registry.CreateEntity();
    registry.AddComponent<PositionComponent>(e, PositionComponent{});
    registry.AddComponent<VelocityComponent>(e, VelocityComponent{});

    registry.DestroyEntity(e);

    TEST_CHECK(!registry.HasComponent<PositionComponent>(e), "Destroying entity should remove position component.");
    TEST_CHECK(!registry.HasComponent<VelocityComponent>(e), "Destroying entity should remove velocity component.");
}

void RegistryTests() {
    RunNamedTest("RegistryTests::TestEntityReuse", &TestEntityReuse);
    RunNamedTest("RegistryTests::TestAddGetComponent", &TestAddGetComponent);
    RunNamedTest("RegistryTests::TestIteration", &TestIteration);
    RunNamedTest("RegistryTests::TestDestroyRemovesComponents", &TestDestroyRemovesComponents);
}