#ifndef ECS_REGISTRY_H
#define ECS_REGISTRY_H

#include "Entity.h"
#include "ComponentPool.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <type_traits>
#include <cassert>

namespace ECS {

class Registry {
public:
	Registry();
	~Registry();

	Entity CreateEntity();
	void DestroyEntity(Entity entity);
	bool IsAlive(Entity entity) const;
	void Clear();

	template<typename T, typename... Args>
	T& AddComponent(Entity entity, Args&&... args) {
	assert(IsAlive(entity) && "Cannot add component to dead entity");

	auto& pool = GetOrCreatePool<T>();
	return pool.Emplace(entity, std::forward<Args>(args)...);
}

	template<typename T>
	bool HasComponent(Entity entity) const {
		auto it = pools_.find(std::type_index(typeid(T)));
		if (it == pools_.end()) {
			return false;
		}

		auto* pool = static_cast<ComponentPool<T>*>(it->second.get());
		return pool->Has(entity);
	}

	template<typename T>
	T& GetComponent(Entity entity) {
		auto& pool = GetOrCreatePool<T>();
		return pool.Get(entity);
	}

	template<typename T>
	const T& GetComponent(Entity entity) const {
		auto it = pools_.find(std::type_index(typeid(T)));
		assert(it != pools_.end() && "Component not registered");
		auto* pool = static_cast<ComponentPool<T>*>(it->second.get());
		return pool->Get(entity);
	}

	template<typename T>
	void RemoveComponent(Entity entity) {
		auto it = pools_.find(std::type_index(typeid(T)));
		if (it == pools_.end()) {
			return;
		}
		auto* pool = static_cast<ComponentPool<T>*>(it->second.get());
		pool->Remove(entity);
	}

	template<typename T, typename Func>
	void ForEach(Func&& func) {
		auto it = pools_.find(std::type_index(typeid(T)));
		if (it == pools_.end()) {
			return;
		}

		auto* pool = static_cast<ComponentPool<T>*>(it->second.get());
		pool->ForEach(std::forward<Func>(func));
	}

private:
	struct EntitySlot {
		bool alive = false;
	};

	template<typename T>
	ComponentPool<T>& GetOrCreatePool() const {
		auto type = std::type_index(typeid(T));
		auto iter = pools_.find(type);
		if (iter == pools_.end()) {
			auto pool = std::make_unique<ComponentPool<T>>();
			auto* ptr = pool.get();
			pools_.emplace(type, std::move(pool));
			return *ptr;
		}
		return *static_cast<ComponentPool<T>*>(iter->second.get());
	}

	std::vector<EntitySlot> entities_;
	std::vector<std::uint32_t> free_list_;
	mutable std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> pools_;
};

} // namespace ECS

#endif // ECS_REGISTRY_H

