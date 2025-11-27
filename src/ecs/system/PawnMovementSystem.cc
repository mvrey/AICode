//------------------------------------------------------------------------------
// File: PawnMovementSystem.cc
// Purpose: Integrates pawn transforms using their direction vectors and
//          configured movement speeds.
//------------------------------------------------------------------------------
#include "../../../include/ecs/system/PawnMovementSystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/MovementComponent.h"
#include "../../../include/ecs/components/TransformComponent.h"
#include "../../../include/GameStatus.h"
#include "../../../include/Camera.h"
#include "../../../include/Pathfinding/cost_map.h"
#include "../../../include/config.h"

#include <MOMOS/math.h>
#include <algorithm>

namespace ECS {

// Moves every entity that has both Movement and Transform components.
void PawnMovementSystem::Update(Registry& registry, double delta_time) {
	CostMap* map = GameStatus::get()->map;
	if (map == nullptr) {
		return;
	}

	registry.ForEach<MovementComponent>([&](Entity entity, MovementComponent& movement) {
		if (!registry.HasComponent<TransformComponent>(entity))
			return;

		auto& transform = registry.GetComponent<TransformComponent>(entity);

		// Get the cell at the pawn's current position to determine speed multiplier
		float speed_multiplier = 1.0f;
		// Calculate tile world dimensions
		float tile_world_width = static_cast<float>(Screen::width) / static_cast<float>(map->getWidth());
		float tile_world_height = static_cast<float>(Screen::height) / static_cast<float>(map->getHeight());

		// Convert world coordinates to map coordinates
		int map_x = static_cast<int>(transform.position.x / tile_world_width);
		int map_y = static_cast<int>(transform.position.y / tile_world_height);

		// Clamp to valid map bounds
		map_x = std::max(0, std::min(map_x, map->getWidth() - 1));
		map_y = std::max(0, std::min(map_y, map->getHeight() - 1));

		// Get the cell and its cost
		Cell* cell = map->getCellAt(map_x, map_y);
		// Apply speed multiplier based on cell cost
		float effective_speed = movement.speed * (1.0f - cell->cost_);

		transform.position.x += transform.direction.x * effective_speed * static_cast<float>(delta_time);
		transform.position.y += transform.direction.y * effective_speed * static_cast<float>(delta_time);
	});
}

} // namespace ECS

