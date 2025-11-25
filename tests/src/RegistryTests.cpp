#include "../../include/ecs/Registry.h"

#include <cassert>
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
	assert(e1 != e2);

	registry.DestroyEntity(e1);
	Entity e3 = registry.CreateEntity();

	assert(e3 == e1 && "Entity ids should be reused after destruction");
}

void TestAddGetComponent() {
	Registry registry;
	Entity e = registry.CreateEntity();

	auto& pos = registry.AddComponent<PositionComponent>(e, PositionComponent{ 3.0f, 4.0f });
	assert(pos.x == 3.0f && pos.y == 4.0f);

	assert(registry.HasComponent<PositionComponent>(e));
	const auto& fetched = registry.GetComponent<PositionComponent>(e);
	assert(fetched.x == 3.0f && fetched.y == 4.0f);

	registry.RemoveComponent<PositionComponent>(e);
	assert(!registry.HasComponent<PositionComponent>(e));
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
		assert(entity.IsValid());
		pos.x += 1.0f;
		pos.y += 1.0f;
		++count;
	});

	assert(count == 3);
	assert(registry.GetComponent<PositionComponent>(e1).x == 2.0f);
	assert(registry.GetComponent<PositionComponent>(e3).y == 7.0f);
}

void TestDestroyRemovesComponents() {
	Registry registry;
	Entity e = registry.CreateEntity();
	registry.AddComponent<PositionComponent>(e, PositionComponent{});
	registry.AddComponent<VelocityComponent>(e, VelocityComponent{});

	registry.DestroyEntity(e);

	assert(!registry.HasComponent<PositionComponent>(e));
	assert(!registry.HasComponent<VelocityComponent>(e));
}

int main() {
	TestEntityReuse();
	TestAddGetComponent();
	TestIteration();
	TestDestroyRemovesComponents();

	std::puts("All ECS registry tests passed.");
	return 0;
}

