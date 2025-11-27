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
#include "../../../include/Pathfinding/path.h"
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

		// Get the cost of the current path step (if following a path)
		float speed_multiplier = 1.0f;
		if (movement.path_set && movement.movement_path != nullptr && 
		    !movement.movement_path->path_.empty() && 
		    movement.deterministic_step_index < movement.movement_path->path_.size()) {
			
			// Get the current path step in map coordinates
			::MOMOS::Vec2 path_step_map = movement.movement_path->path_[movement.deterministic_step_index];
			
			// Get the cell at the current path step
			Cell* cell = map->getCellAt(static_cast<int>(path_step_map.x), static_cast<int>(path_step_map.y));
			if (cell != nullptr) {
				// Use the cell's cost to modify speed: cost_ = 0.0f means full speed, higher cost = slower
				// Formula: speed_multiplier = 1.0f - cost_
				speed_multiplier = 1.0f - cell->cost_;
			}
		}

		// Apply speed multiplier based on path step cost
		float effective_speed = movement.speed * speed_multiplier;

		transform.position.x += transform.direction.x * effective_speed * static_cast<float>(delta_time);
		transform.position.y += transform.direction.y * effective_speed * static_cast<float>(delta_time);
	});
}

} // namespace ECS

