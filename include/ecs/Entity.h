//------------------------------------------------------------------------------
// File: Entity.h
// Purpose: Defines the lightweight entity identifier used to connect component
//          pools in the ECS registry.
//------------------------------------------------------------------------------
#ifndef ECS_ENTITY_H
#define ECS_ENTITY_H

#include <cstdint>
#include <cstddef>
#include <functional>
#include <climits>

namespace ECS {

// Simple wrapper over an integer id that offers validity helpers and hashing.
struct Entity {
	static constexpr std::uint32_t kInvalidId = static_cast<std::uint32_t>(-1);

	// Constructs an invalid entity for convenience.
	Entity() : id(kInvalidId) {}
	// Explicit constructor so callers can create stable ids from the registry.
	explicit Entity(std::uint32_t value) : id(value) {}

	// Checks whether the entity points to a registered slot.
	bool IsValid() const { return id != kInvalidId; }

	// Entities are equal if their identifiers match.
	bool operator==(const Entity& other) const { return id == other.id; }
	bool operator!=(const Entity& other) const { return !(*this == other); }

	std::uint32_t id;
};

// Hash functor so Entity can serve as unordered_map/unordered_set key.
struct EntityHasher {
	std::size_t operator()(const Entity& entity) const noexcept {
		return std::hash<std::uint32_t>()(entity.id);
	}
};

} // namespace ECS

#endif // ECS_ENTITY_H

