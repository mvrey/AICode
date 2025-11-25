//------------------------------------------------------------------------------
// File: Registry.h
// Purpose: Declares the central ECS registry responsible for creating entities
//          and managing component pools.
//------------------------------------------------------------------------------
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

	// Owns all entities and their component pools, providing APIs for creation,
	// destruction, and iteration.
	class Registry {
	public:
		// Initializes empty storage for entities/pools.
		Registry();
		// Ensures pools release memory before shutdown.
		~Registry();

		// Allocates a new entity id, reusing free slots when available.
		Entity CreateEntity();
		// Marks an entity as dead and notifies every pool to remove its data.
		void DestroyEntity(Entity entity);
		// Checks whether the entity is alive and available for queries.
		bool IsAlive(Entity entity) const;
		// Clears all entities and component pools, returning to a fresh state.
		void Clear();

		// Adds (or replaces) a component of type T on the provided entity.
		template<typename T, typename... Args>
		T& AddComponent(Entity entity, Args&&... args) {
			assert(IsAlive(entity) && "Cannot add component to dead entity");

			auto& pool = GetOrCreatePool<T>();
			return pool.Emplace(entity, std::forward<Args>(args)...);
		}

		// Returns true when the entity currently has a component of type T.
		template<typename T>
		bool HasComponent(Entity entity) const {
			auto it = pools_.find(std::type_index(typeid(T)));
			if (it == pools_.end()) {
				return false;
			}

			auto* pool = static_cast<ComponentPool<T>*>(it->second.get());
			return pool->Has(entity);
		}

		// Retrieves a mutable component reference, creating the pool if needed.
		template<typename T>
		T& GetComponent(Entity entity) {
			auto& pool = GetOrCreatePool<T>();
			return pool.Get(entity);
		}

		// Retrieves a const component reference, asserting it exists.
		template<typename T>
		const T& GetComponent(Entity entity) const {
			auto it = pools_.find(std::type_index(typeid(T)));
			assert(it != pools_.end() && "Component not registered");
			auto* pool = static_cast<ComponentPool<T>*>(it->second.get());
			return pool->Get(entity);
		}

		// Removes the specified component type from the entity if present.
		template<typename T>
		void RemoveComponent(Entity entity) {
			auto it = pools_.find(std::type_index(typeid(T)));
			if (it == pools_.end()) {
				return;
			}
			auto* pool = static_cast<ComponentPool<T>*>(it->second.get());
			pool->Remove(entity);
		}

		// Iterates all components of type T, invoking the callback per entity.
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

		// GetOrCreatePool NO es const: modifica pools_ cuando crea una nueva pool.
		// Lazily builds the pool for T and returns it so callers can store data.
		template<typename T>
		ComponentPool<T>& GetOrCreatePool() {
			auto type = std::type_index(typeid(T));
			auto iter = pools_.find(type);
			if (iter == pools_.end()) {
				// Construir directamente un unique_ptr<IComponentPool> evita conversiones ambiguas
				std::unique_ptr<IComponentPool> pool(new ComponentPool<T>());
				auto* ptr = static_cast<ComponentPool<T>*>(pool.get());
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