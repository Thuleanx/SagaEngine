#pragma once

#include "componentGroup.h"
#include <type_traits>
#include <utility>
#include "../_Core/logger.h"

namespace Saga {

template <typename... Component>
void ComponentGroup<Component...>::addEntity(std::shared_ptr<GameWorld> world, const Entity& entity) {
	if (entityToIndex.count(entity)) {
		SWARN("Trying to add an entity %d to group, but this group already has it.", entity);
		return;
	}

	allData.emplace_back(std::move(createTuple(world, entity)));
	entityToIndex[entity] = allData.size()-1;
}

template <typename... Component>
void ComponentGroup<Component...>::removeEntity(Entity entity) {
	if (!entityToIndex.count(entity)) {
		SWARN("Trying to remove an entity %d from group, but this group does not have it.", entity);
		return;
	}
	int indexToRemove = entityToIndex[entity];
	Entity entityToReplace = get<0>(allData.back());

	// removing this in place will create a hole in our vector of tuples, so we swap with the back element, and then remove
	std::swap(allData[indexToRemove], allData[allData.size()-1]);
	entityToIndex[entityToReplace] = indexToRemove;
	entityToIndex.erase(entity);
	allData.pop_back();
}

template <typename... Component>
std::tuple<Entity, ComponentReference<Component>...> ComponentGroup<Component...>::createTuple(
	std::shared_ptr<GameWorld> world, const Entity& entity) {
    return std::make_tuple(entity, world->getComponent<Component>(entity)...);
}

template <typename... Component>
typename std::vector<std::tuple<Entity, ComponentReference<Component>...>>::iterator ComponentGroup<Component...>::begin() { return allData.begin();}

template <typename... Component>
typename std::vector<std::tuple<Entity, ComponentReference<Component>...>>::iterator ComponentGroup<Component...>::end() { return allData.end();}

template <typename... Component>
typename std::vector<std::tuple<Entity, ComponentReference<Component>...>>::const_iterator ComponentGroup<Component...>::begin() const { return allData.begin();}

template <typename... Component>
typename std::vector<std::tuple<Entity, ComponentReference<Component>...>>::const_iterator ComponentGroup<Component...>::end() const { return allData.end();}

}
