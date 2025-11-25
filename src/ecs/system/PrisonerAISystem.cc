//------------------------------------------------------------------------------
// File: PrisonerAISystem.cc
// Purpose: Implements the prisoner AI state machine using ECS components,
//          replacing the legacy AgentMind logic.
//------------------------------------------------------------------------------
#include "../../../include/ecs/system/PrisonerAISystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/PrisonerComponents.h"
#include "../../../include/ecs/PrisonerMovementUtils.h"
#include "../../../include/GameStatus.h"
#include "../../../include/PrisonMap.h"
#include "../../../include/Pathfinding/cost_map.h"
#include "../../../include/config.h"

#include <MOMOS/math.h>

namespace {

namespace MovementUtils = PrisonerECS::MovementUtils;

inline bool HasArrived(const ECS::MovementComponent& movement) {
	return movement.movement_finished && !movement.path_set;
}

} // namespace

namespace ECS {

void PrisonerAISystem::Update(Registry& registry, double /*delta_time*/) {
	auto* status = GameStatus::get();
	if (!status || !status->prison || !status->map) {
		return;
	}

	PrisonMap* prison = status->prison;
	CostMap* map = status->map;

	registry.ForEach<PrisonerStateComponent>([&](Entity entity, PrisonerStateComponent& state) {
		if (!registry.HasComponent<MovementComponent>(entity) ||
			!registry.HasComponent<TransformComponent>(entity)) {
			return;
		}

		auto& movement = registry.GetComponent<MovementComponent>(entity);
		auto& transform = registry.GetComponent<TransformComponent>(entity);

		auto clearMovement = [&]() {
			MovementUtils::ClearMovement(registry, entity);
		};

		auto goToRoom = [&](const Room& room) -> bool {
			if (HasArrived(movement)) {
				return true;
			}

			if (movement.path_set) {
				return false;
			}

			if (MovementUtils::TryFinalizePath(registry, entity)) {
				return false;
			}

			if (MovementUtils::BuildRoomWaypointPath(registry, entity, room, prison, map)) {
				return false;
			}

			MovementUtils::RequestPathTo(registry, entity, map->MapToScreenCoords(prison->getRandomPointInRoom(room)));
			return false;
		};

		switch (state.status) {
		case kIdle:
			transform.direction = { 0.0f, 0.0f };
			movement.speed = state.original_speed;
			state.status = kGoingToRest;
			break;
		case kGoingToWork:
			if (status->working_shift_ != state.working_shift) {
				state.status = kGoingToRest;
				break;
			}
			if (goToRoom(prison->loading_area_)) {
				clearMovement();
				state.status = kWorkingLoaded;
				state.time_end_status = status->game_time + 5000.0;
			}
			break;
		case kWorkingLoaded:
			if (status->working_shift_ != state.working_shift) {
				movement.speed = state.original_speed;
				state.status = kGoingToRest;
				clearMovement();
				break;
			}

			if (status->first_available_crate_index < 100) {
				if (state.carried_crate == nullptr) {
					status->first_available_crate_index++;
					movement.speed = state.original_speed * 0.5f;
				}

				if (goToRoom(prison->unloading_area_)) {
					clearMovement();
					state.status = kWorkingUnloaded;
					state.time_end_status = status->game_time + 5000.0;
				}
			} else {
				state.status = kGoingToRest;
			}
			break;
		case kWorkingUnloaded:
			if (state.carried_crate != nullptr) {
				state.carried_crate = nullptr;
				movement.speed = state.original_speed;
			}

			if (status->working_shift_ != state.working_shift) {
				clearMovement();
				state.status = kGoingToRest;
				movement.speed = state.original_speed;
				break;
			}

			if (goToRoom(prison->loading_area_)) {
				clearMovement();
				state.status = kWorkingLoaded;
				state.time_end_status = status->game_time + 5000.0;
			}
			break;
		case kGoingToRest:
			if (goToRoom(prison->resting_room_)) {
				clearMovement();
				state.status = kResting;
			}
			break;
		case kResting:
			if (status->working_shift_ == state.working_shift) {
				clearMovement();
				state.status = kGoingToWork;
			} else {
				goToRoom(prison->resting_room_);
			}
			break;
		case kEscaping: {
			movement.speed = state.original_speed;
			MovementUtils::SetEscapeRouteActive(registry, entity, true);

			PrisonAreaType area = prison->getAreaTypeAt(transform.position);
			if (area == kBase) {
				MovementUtils::SetEscapeRouteActive(registry, entity, false);
				clearMovement();
				state.status = kIdle;
				break;
			}

			if (!movement.path_set) {
				if (!movement.door_route_set) {
					clearMovement();
					MovementUtils::SetDoorRouteActive(registry, entity, true);
				}
			}
			break;
		}
		default:
			break;
		}
	});
}

} // namespace ECS

