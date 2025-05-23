#pragma once

#include "managers/componentManager.hpp"
#include "managers/entityManager.hpp"

#include <SDL3/SDL.h>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace utils {

template <class...> struct False : std::bool_constant<false> {};

template <typename Iterator> struct iterable_adaptor final {
	constexpr iterable_adaptor(Iterator from, Iterator to) noexcept : first{std::move(from)}, last{std::move(to)} {}

	[[nodiscard]] constexpr Iterator begin() const noexcept { return first; }
	[[nodiscard]] constexpr Iterator cbegin() const noexcept { return begin(); }

	[[nodiscard]] constexpr Iterator end() const noexcept { return last; }
	[[nodiscard]] constexpr Iterator cend() const noexcept { return end(); }

      private:
	Iterator first;
	Iterator last;
};

template <typename... ComponentTypes> class sparse_set_view_tuple_iterator final {
      public:
	// mEntities is copied, but it shouldn't - bad for perf
	sparse_set_view_tuple_iterator(ComponentManager* componentManager, const std::vector<EntityID>& entities,
				       std::size_t offset) noexcept
		: mComponentManager(componentManager), mEntities(entities), mOffset(offset) {}

	sparse_set_view_tuple_iterator& operator++() noexcept { return ++mOffset, *this; }

	sparse_set_view_tuple_iterator& operator--() noexcept { return --mOffset, *this; }

	[[nodiscard]] decltype(auto) operator[](const size_t value) const noexcept {
		return std::make_tuple(mEntities[index() + value], mComponentManager->getPool<ComponentTypes>()->get(
									   mEntities[index() + value])...);
	}

	[[nodiscard]] decltype(auto) operator*() const noexcept { return operator[](0); }

	[[nodiscard]] size_t index() const noexcept { return mOffset; }

      protected:
	class ComponentManager* mComponentManager;
	const std::vector<EntityID> mEntities;
	std::size_t mOffset;
};

template <typename... ComponentTypes>
[[nodiscard]] bool operator==(const sparse_set_view_tuple_iterator<ComponentTypes...>& lhs,
			      const sparse_set_view_tuple_iterator<ComponentTypes...>& rhs) noexcept {
	return lhs.index() == rhs.index();
}

template <typename... ComponentTypes>
[[nodiscard]] bool operator!=(const sparse_set_view_tuple_iterator<ComponentTypes...>& lhs,
			      const sparse_set_view_tuple_iterator<ComponentTypes...>& rhs) noexcept {
	return !(lhs == rhs);
}

template <typename T>
concept is_emplace_constructible = requires(std::allocator<T> m, T* p, T args) {
	std::allocator_traits<std::allocator<T>>::construct(m, p, args);
};

template <typename... ComponentTypes>
	requires((std::is_move_assignable<ComponentTypes>::value && ...) &&
		 (utils::is_emplace_constructible<ComponentTypes> && ...))
class sparse_set_view {
      public:
	using iterator = std::vector<EntityID>::iterator;
	using const_iterator = std::vector<EntityID>::const_iterator;
	using tuple_iterator = sparse_set_view_tuple_iterator<ComponentTypes...>;
	using iterable = iterable_adaptor<sparse_set_view_tuple_iterator<ComponentTypes...>>;

	// PERF: Large bottleneck
	sparse_set_view() noexcept {
		if constexpr (sizeof...(ComponentTypes) == 1) {
			const auto& c = *(static_cast<utils::sparse_set<ComponentTypes>*>(
						  ComponentManager::getInstance()->getPool<ComponentTypes>()),
					  ...);
			mEntities = std::vector<EntityID>(c.begin(), c.end());
		} else {
			// This first part makes a array of all the sizes of the that we loop through
			const std::array<utils::sparse_set_interface*, sizeof...(ComponentTypes)> sets = {
				ComponentManager::getInstance()->getPool<ComponentTypes>()...};

			std::size_t smallest = 0;
			std::size_t smallest_size = sets[0]->size();
			for (std::size_t i = 0; i < sizeof...(ComponentTypes); ++i) {
				if (sets[i]->size() < smallest_size) {
					smallest = i;
					smallest_size = sets[i]->size();
				}
			}

			mEntities.reserve(smallest_size);

			for (const auto id : *sets[smallest]) {
				if constexpr (sizeof...(ComponentTypes) == 1) {
					mEntities.emplace_back(id);
				} else {
					if ((... &&
					     (ComponentManager::getInstance()->getPool<ComponentTypes>()->contains(
						     id)))) {
						mEntities.emplace_back(id);
					}
				}
			}
		}
		// Now at the end of the program, all the entites that are present in all the components are
		// present in mEntities (doesn't count in addition and deletion)
	}

	// Copy
	sparse_set_view(const sparse_set_view& other) noexcept = default;
	sparse_set_view& operator=(const sparse_set_view& other) noexcept = default;

	// Move
	sparse_set_view(sparse_set_view&& other) noexcept = default;
	sparse_set_view& operator=(sparse_set_view&& other) noexcept = default;

	~sparse_set_view() {}

	[[nodiscard]] iterator begin() noexcept { return mEntities.begin(); }
	[[nodiscard]] const_iterator begin() const noexcept { return mEntities.begin(); }
	[[nodiscard]] const_iterator cbegin() const noexcept { return begin(); }

	[[nodiscard]] iterator end() noexcept { return mEntities.end(); }
	[[nodiscard]] const_iterator end() const noexcept { return mEntities.end(); }
	[[nodiscard]] const_iterator cend() const noexcept { return end(); }

	[[nodiscard]] iterable each() const noexcept {
		return iterable{tuple_iterator{ComponentManager::getInstance(), mEntities, 0},
				tuple_iterator{ComponentManager::getInstance(), mEntities, mEntities.size()}};
	}

	template <typename... Components> [[nodiscard]] decltype(auto) get(const EntityID entt) const {
		if constexpr (sizeof...(Components) == 1) {
			return (ComponentManager::getInstance()->getPool<Components>()->get(entt), ...);
		} else {
			return std::make_tuple(ComponentManager::getInstance()->getPool<Components>()->get(entt)...);
		}
	}

	template <typename Func> void each(Func func) const {
		if constexpr (std::is_invocable_v<Func, EntityID>) {
			for (const auto entity : mEntities) {
				func(entity);
			}
		} else if constexpr (std::is_invocable_v<Func, EntityID, ComponentTypes&...>) {
			for (const auto entity : mEntities) {
				func(entity,
				     ComponentManager::getInstance()->getPool<ComponentTypes>()->get(entity)...);
			}
		} else if constexpr (std::is_invocable_v<Func, ComponentTypes&...>) {
			for (const auto entity : mEntities) {
				func(ComponentManager::getInstance()->getPool<ComponentTypes>()->get(entity)...);
			}
		} else {
			static_assert(False<Func>{}, "The signatures for each are: (EntityID), (EntityID, ComponentTypes&...) "
					     "and (ComponentTypes&...)");
			std::unreachable();
		}
	}

	std::size_t size() const noexcept { return mEntities.size(); }

	constexpr EntityID* data() noexcept { return mEntities.data(); }
	constexpr const EntityID* data() const noexcept { return mEntities.data(); }

      private:
	std::vector<EntityID> mEntities;
};

} // namespace utils
