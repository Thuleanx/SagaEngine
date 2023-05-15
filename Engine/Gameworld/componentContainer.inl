#pragma once

#include "componentContainer.h"
#include <iostream>
#include <stdexcept>
#include "../_Core/asserts.h"

namespace Saga {

template <typename Component>
typename std::vector<Component>::iterator ComponentContainer<Component>::begin() { return components.begin(); }

template <typename Component>
typename std::vector<Component>::iterator ComponentContainer<Component>::end() { return components.begin() + cnt; }

template <typename Component>
typename std::vector<Component>::const_iterator ComponentContainer<Component>::begin() const { return components.begin(); }

template <typename Component>
typename std::vector<Component>::const_iterator ComponentContainer<Component>::end() const { return components.begin() + cnt; }

template <typename Component>
template <typename... Args>
Component* ComponentContainer<Component>::emplace(const Entity entity, Args &&...args) {
    int index = cnt++;
	// repack if not enough space
    while (index >= components.size()) tryRepack();
    components[index] = Component(args...);

	SASSERT_DEBUG_MESSAGE(!componentMap.count(entity), "Entity already has a component of the same type attached. You cannot have multiple of the same component type attached to the same entity.");

	// adjust the relevant maps
	componentMap[entity] = index;
	entityMap[index] = entity;

	tryRepack();

	return &components[index];
}

template <typename Component>
std::optional<Component*> ComponentContainer<Component>::any() {
    if (begin() == end()) return {};
    return &*begin();
}

template <typename Component>
Component* ComponentContainer<Component>::getComponent(const Entity entity) {
	if (!componentMap.count(entity)) return nullptr;
	int index = componentMap[entity];
    return &components[index];
}

template <typename Component>
bool ComponentContainer<Component>::hasComponent(const Entity entity) {
	return componentMap.count(entity);
}

template <typename Component>
Entity ComponentContainer<Component>::getEntity(Component* component) {
	// find an iterator to the position in the vertex with the same component pointer
    auto iter = std::find_if(components.begin(), components.end(), [&](Component& value) {
		return &value == component; // compare pointers
	});
	if (iter != components.end()) {
		int index = iter - components.begin(); // we can find index this way because we use a vector
		return entityMap[index];
	}
    return (Entity) -1; // if entity not found
}

template <typename Component>
void ComponentContainer<Component>::onEntityDestroyed(Entity entity) {
	removeComponent(entity);
}

template <typename Component>
void ComponentContainer<Component>::removeComponent(const Entity entity) {
	if (!componentMap.count(entity)) return;

    int componentIndex = componentMap[entity];
	// first remove the entity

	SASSERT_MESSAGE(cnt >= 0, "Number of components decreased below 0. This should not be possible.");

	if (cnt > 0) {
		// this deletion creates a hole in our component list, we want to adjust that by 
		// swapping the last entity + component to this hole index
		Entity entityLast = entityMap[cnt-1];

		// and adjust the corresponding maps
		componentMap[entityLast] = componentIndex;
		entityMap[componentIndex] = entityLast;

        STRACE("swapped %d %d", cnt-1, componentIndex);
        std::swap(components[cnt-1], components[componentIndex]);

        // signal that pointers are bad
        lastReallocated++;
	}


	componentMap.erase(entity); 
	entityMap.erase(cnt-1); // and decrement the size
    cnt--;

	// repack, if too big
	tryRepack();
}

template <typename Component>
void ComponentContainer<Component>::tryRepack() {
	// half the size if more than 3/4 of the vector is unused
	if (components.size() > 4 * cnt) {
		components.resize(cnt*2);
		lastReallocated++;
	} 
	// double the size if about to overflow
	else if (components.size() <= cnt) {
		components.resize(cnt*2);
		lastReallocated++;
	}
}

} // namespace Saga
