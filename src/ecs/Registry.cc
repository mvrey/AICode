//------------------------------------------------------------------------------
// File: Registry.cc
// Purpose: Implements the non-templated portions of the ECS registry such as
//          entity lifecycle management.
//------------------------------------------------------------------------------
#include "../../include/ecs/Registry.h"

namespace ECS {

Registry::Registry() = default;
Registry::~Registry() = default;

// Grabs an id from the free list or appends a new slot when necessary.
Entity Registry::CreateEntity() {
	std::uint32_t id;
	if (!free_list_.empty()) {
		id = free_list_.back();
		free_list_.pop_back();
		entities_[id].alive = true;
	} else {
		id = static_cast<std::uint32_t>(entities_.size());
		entities_.push_back({ true });
	}
	return Entity(id);
}

// Marks the entity as dead and notifies every component pool.
void Registry::DestroyEntity(Entity entity) {
	if (!IsAlive(entity)) {
		return;
	}

	entities_[entity.id].alive = false;
	free_list_.push_back(entity.id);

	for (auto& entry : pools_) {
		entry.second->OnEntityDestroyed(entity);
	}
}

// Verifies the entity id is in range and currently marked alive.
bool Registry::IsAlive(Entity entity) const {
	return entity.id < entities_.size() && entities_[entity.id].alive;
}

// Resets every container so the registry returns to a pristine state.
void Registry::Clear() {
	entities_.clear();
	free_list_.clear();
	pools_.clear();
}

} // namespace ECS

