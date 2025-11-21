#ifndef ECS_ENTITY_H
#define ECS_ENTITY_H

#include <cstdint>
#include <cstddef>
#include <functional>
#include <climits>

namespace ECS {

struct Entity {
	static constexpr std::uint32_t kInvalidId = static_cast<std::uint32_t>(-1);

	Entity() : id(kInvalidId) {}
	explicit Entity(std::uint32_t value) : id(value) {}

	bool IsValid() const { return id != kInvalidId; }

	bool operator==(const Entity& other) const { return id == other.id; }
	bool operator!=(const Entity& other) const { return !(*this == other); }

	std::uint32_t id;
};

struct EntityHasher {
	std::size_t operator()(const Entity& entity) const noexcept {
		return std::hash<std::uint32_t>()(entity.id);
	}
};

} // namespace ECS

#endif // ECS_ENTITY_H

