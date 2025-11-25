//------------------------------------------------------------------------------
// File: ComponentPool.h
// Purpose: Defines the sparse-set storage that backs ECS component pools, so
//          systems can add/remove/query components in O(1) while keeping data
//          contiguous for cache-friendly iteration.
//------------------------------------------------------------------------------
#ifndef ECS_COMPONENT_POOL_H
#define ECS_COMPONENT_POOL_H

#include "Entity.h"

#include <vector>
#include <memory>
#include <cstddef>

namespace ECS {

// Provides a minimal interface so different pools can be stored generically.
class IComponentPool {
public:
	// Virtual dtor allows derived pools to clean up component storage.
	virtual ~IComponentPool() = default;
	// Removes the component owned by the provided entity if it exists.
	virtual void Remove(Entity entity) = 0;
	// Reports whether the entity currently owns a component of this type.
	virtual bool Has(Entity entity) const = 0;
	// Default hook invoked when an entity is destroyed to release its payload.
	virtual void OnEntityDestroyed(Entity entity) { Remove(entity); }
};

template<typename T>
class ComponentPool : public IComponentPool {
public:
	// Pools default-construct empty and grow as entities are added.
	ComponentPool() = default;

	// Constructs (or overwrites) a component for an entity in-place while
	// maintaining dense storage for cache-friendly iteration.
	template<typename... Args>
	T& Emplace(Entity entity, Args&&... args) {
		EnsureSparseSize(entity.id + 1);

		std::size_t index = sparse_[entity.id];
		if (index != kInvalidIndex) {
			components_[index] = T(std::forward<Args>(args)...);
			return components_[index];
		}

		index = components_.size();
		components_.emplace_back(std::forward<Args>(args)...);
		owners_.push_back(entity);
		sparse_[entity.id] = index;
		return components_.back();
	}

	// Removes the entity's component by swapping with the dense tail and
	// updating sparse lookups so iteration stays compact.
	void Remove(Entity entity) override {
		if (entity.id >= sparse_.size())
			return;

		std::size_t index = sparse_[entity.id];
		if (index == kInvalidIndex)
			return;

		std::size_t last_index = components_.size() - 1;
		if (index != last_index) {
			std::swap(components_[index], components_[last_index]);
			std::swap(owners_[index], owners_[last_index]);
			sparse_[owners_[index].id] = index;
		}

		components_.pop_back();
		owners_.pop_back();
		sparse_[entity.id] = kInvalidIndex;
	}

	// Constant-time lookup to check if an entity is registered in the pool.
	bool Has(Entity entity) const override {
		return entity.id < sparse_.size() && sparse_[entity.id] != kInvalidIndex;
	}

	// Returns a mutable reference to the entity's component, throwing if absent.
	T& Get(Entity entity) {
		return components_.at(sparse_.at(entity.id));
	}

	// Returns a const view to the entity's component, throwing if absent.
	const T& Get(Entity entity) const {
		return components_.at(sparse_.at(entity.id));
	}

	// Iterates over every stored component, exposing its owning entity.
	template<typename Func>
	void ForEach(Func&& func) {
		for (std::size_t i = 0; i < components_.size(); ++i) {
			func(owners_[i], components_[i]);
		}
	}

	// Ensures dangling components are cleaned up when an entity is removed.
	void OnEntityDestroyed(Entity entity) override {
		Remove(entity);
	}

private:
	static constexpr std::size_t kInvalidIndex = static_cast<std::size_t>(-1);

	// Grows the sparse lookup vector so future entity ids map safely.
	void EnsureSparseSize(std::size_t size) {
		if (sparse_.size() < size) {
			sparse_.resize(size, kInvalidIndex);
		}
	}

	std::vector<T> components_;
	std::vector<Entity> owners_;
	std::vector<std::size_t> sparse_;
};

} // namespace ECS

#endif // ECS_COMPONENT_POOL_H

