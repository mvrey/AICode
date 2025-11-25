#include "../../../include/ecs/system/PrisonerAISystem.h"
#include "../../../include/ecs/Registry.h"
#include "../../../include/ecs/components/PrisonerComponents.h"
#include "../../../include/ecs/PrisonerMovementUtils.h"
#include "../../../include/Agents/Prisoner.h"
#include "../../../include/GameStatus.h"
#include "../../../include/PrisonMap.h"
#include "../../../include/Pathfinding/cost_map.h"
#include "../../../include/config.h"

#include <MOMOS/math.h>

namespace {

inline float ClampPositive(float value) {
	return value < 0.0f ? 0.0f : value;
}

} // namespace

namespace ECS {

void PrisonerAISystem::Update(Registry& registry, double /*delta_time*/) {
	GameStatus* status = GameStatus::get();
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

		auto& transform = registry.GetComponent<TransformComponent>(entity);
		auto& movement = registry.GetComponent<MovementComponent>(entity);
		Prisoner* owner = state.owner;

		auto clearMovement = [&]() {
			if (owner) {
				PrisonerECS::MovementUtils::ClearMovement(*owner);
			} else {
				movement.deterministic_steps.clear();
				movement.deterministic_step_index = 0;
				movement.path_set = false;
				movement.movement_finished = false;
			}
		};

		auto goToRoom = [&](const Room& room) -> bool {
			if (!owner) {
				return false;
			}
			return owner->goToRoom(room);
		};

		auto moveFollowingPath = [&]() -> bool {
			if (!owner) {
				return movement.movement_finished;
			}
			return PrisonerECS::MovementUtils::MoveFollowingPath(*owner);
		};

		auto setPathTo = [&](const ::MOMOS::Vec2& dest) {
			if (owner) {
				PrisonerECS::MovementUtils::SetPathTo(*owner, dest);
			}
		};

		auto setDoorRouteActive = [&](bool active) {
			movement.door_route_set = active;
			if (owner) {
				PrisonerECS::MovementUtils::SetDoorRouteActive(*owner, active);
			}
		};

		auto setEscapeRouteActive = [&](bool active) {
			movement.escape_route_set = active;
			if (owner) {
				PrisonerECS::MovementUtils::SetEscapeRouteActive(*owner, active);
			}
		};

		auto cycleDoorTarget = [&](int totalDoors) {
			if (totalDoors <= 0) {
				return;
			}
			movement.current_target_door = (movement.current_target_door + 1) % totalDoors;
			if (owner) {
				PrisonerECS::MovementUtils::CycleDoorTarget(*owner, totalDoors);
			}
		};

		auto setDoorTarget = [&](int index) {
			movement.current_target_door = index;
			if (owner) {
				PrisonerECS::MovementUtils::SetDoorTarget(*owner, index);
			}
		};

		if (status->alarm_mode_ && state.status != kEscaping) {
			clearMovement();
			setEscapeRouteActive(true);
			state.status = kEscaping;
		}

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
					movement.speed = ClampPositive(state.original_speed * 0.5f);
				}

				if (state.carried_crate != nullptr) {
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
				state.status = kGoingToRest;
				clearMovement();
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
			setEscapeRouteActive(true);

			PrisonAreaType area = prison->getAreaTypeAt(transform.position);
			if (area == kBase) {
				if (owner) {
					setEscapeRouteActive(false);
					owner->aliveStatus_ = kDead;
				}
				return;
			}

			if (!status->alarm_mode_ && movement.movement_finished) {
				setEscapeRouteActive(false);
				setDoorRouteActive(false);
				state.status = kGoingToRest;
				break;
			}

			const int door_count = static_cast<int>(prison->doors_.size());

			if (!movement.path_set) {
				if (!movement.door_route_set) {
					clearMovement();
					setDoorRouteActive(true);
				}
				if (door_count > 0) {
					if (movement.current_target_door >= door_count) {
						setDoorTarget(movement.current_target_door % door_count);
					}
					MOMOS::Vec2 dest = map->MapToScreenCoords(
						prison->doors_[movement.current_target_door]->getFrontalPoint(true));
					setPathTo(dest);
				}
			} else {
				if (moveFollowingPath()) {
					if (movement.door_route_set) {
						if (door_count == 0) {
							break;
						}
						if (prison->doors_[movement.current_target_door]->is_open_) {
							MOMOS::Vec2 dest = { static_cast<float>(Screen::width - 100), static_cast<float>(Screen::height - 50) };
							clearMovement();
							setPathTo(dest);
						} else {
							cycleDoorTarget(door_count);
							clearMovement();
							setDoorRouteActive(false);
						}
					}
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

