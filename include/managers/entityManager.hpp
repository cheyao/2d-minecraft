#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <limits>

using EntityID = std::uint64_t;

constexpr const static EntityID MAX_ENTITIES = std::numeric_limits<EntityID>::max();
constexpr const static std::uint64_t MAX_COMPONENT_TYPES = 64;

class EntityManager {
      public:
	EntityManager();
	EntityManager(EntityManager&&) = delete;
	EntityManager(const EntityManager&) = delete;
	EntityManager& operator=(EntityManager&&) = delete;
	EntityManager& operator=(const EntityManager&) = delete;
	~EntityManager() = default;

	[[nodiscard]] EntityID getEntity();
	void releaseEntity(EntityID entity);
	[[nodiscard]] std::size_t getEntityCount() const { return mNext - mReleased.size(); }
	[[nodiscard]] bool valid(const EntityID entity) const;

      private:
	EntityID mNext;
	std::deque<EntityID> mReleased;
};
