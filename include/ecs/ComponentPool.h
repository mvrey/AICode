#ifndef ECS_COMPONENT_POOL_H
#define ECS_COMPONENT_POOL_H

#include "Entity.h"

#include <vector>
#include <memory>
#include <cstddef>

namespace ECS {

class IComponentPool {
public:
	virtual ~IComponentPool() = default;
	virtual void Remove(Entity entity) = 0;
	virtual bool Has(Entity entity) const = 0;
	virtual void OnEntityDestroyed(Entity entity) { Remove(entity); }
};

template<typename T>
class ComponentPool : public IComponentPool {
public:
	ComponentPool() = default;

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

	bool Has(Entity entity) const override {
		return entity.id < sparse_.size() && sparse_[entity.id] != kInvalidIndex;
	}

	T& Get(Entity entity) {
		return components_.at(sparse_.at(entity.id));
	}

	const T& Get(Entity entity) const {
		return components_.at(sparse_.at(entity.id));
	}

	template<typename Func>
	void ForEach(Func&& func) {
		for (std::size_t i = 0; i < components_.size(); ++i) {
			func(owners_[i], components_[i]);
		}
	}

	void OnEntityDestroyed(Entity entity) override {
		Remove(entity);
	}

private:
	static constexpr std::size_t kInvalidIndex = static_cast<std::size_t>(-1);

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

