#pragma once

#include "gameworld.h"
#include "componentContainer.h"
#include "componentGroup.h"
#include "componentReference.h"
#include "../_Core/asserts.h"

namespace Saga {

template <typename Component, typename... Args>
ComponentReference<Component> GameWorld::emplace(const Entity entity, Args &&...args) {
	// guarantee that the component container is not a null reference
	if (!componentMap.hasKey<Component>()) {
		SASSERT_MESSAGE(componentMap.size() < MAX_COMPONENTS, "Too many component types have been added to the World. Consider increasing MAX_COMPONENTS in signature.h.");
		componentMap.put<Component>(std::make_shared<ComponentContainer<Component>>());
	}

	auto it = componentMap.find<Component>();
	// at this point, we know that this container contains component
	Component* component = std::dynamic_pointer_cast<ComponentContainer<Component>>(it->second)->emplace(entity, args...);

	// first obtain a new signature for the entity
	Signature& signature = entitySignatures[entity];
	int componentId = getTypeId<Component>();
	signature[componentId] = true;

	// add entity to the relevant groups. Only consider groups that the entity would not have been in before this.
	for (auto &[groupSignature, group] : componentGroups) 
		if ((signature & groupSignature) == groupSignature && groupSignature[componentId]) 
			group->addEntity(shared_from_this(), entity);

	return ComponentReference<Component>(viewAll<Component>(), entity);
}

template <typename Component>
std::shared_ptr<ComponentContainer<Component>> GameWorld::viewAll() {
	if (!componentMap.hasKey<Component>())
		componentMap.put<Component>(std::make_shared<ComponentContainer<Component>>());
	return std::static_pointer_cast<ComponentContainer<Component>>(componentMap.find<Component>()->second);
}

template<typename Component>
ComponentReference<Component> GameWorld::getComponent(const Entity entity) {
    return ComponentReference<Component>(viewAll<Component>(), entity);
}

template<typename Component>
bool GameWorld::hasComponent(const Entity entity) {
    return viewAll<Component>()->hasComponent(entity);
}

template<typename Component>
Entity GameWorld::getEntity(Component* component) {
    return viewAll<Component>()->getEntity(component);
}

template<typename Component>
void GameWorld::removeComponent(const Entity entity) {
	Signature& signature = entitySignatures[entity];

	int componentId = getTypeId<Component>();

	// find groups that this component no longer belong to a group
    for (auto &[groupSignature, group] : componentGroups)
        if (((signature & groupSignature) == groupSignature) && groupSignature[componentId])
			group->removeEntity(entity);

	// update signature of the entity
	signature[componentId] = false;
    return viewAll<Component>()->removeComponent(entity);
}

template<typename... Component>
std::shared_ptr<ComponentGroup<Component...>> GameWorld::registerGroup() {
	Signature groupSignature = createSignature<Component...>();
	if (!componentGroups.count(groupSignature))
    	componentGroups[groupSignature] = std::make_shared<ComponentGroup<Component...>>();
	else {
        std::string namesOfComponents;
		([&] {
            namesOfComponents += typeid(Component).name();
            namesOfComponents += ", ";
		}(), ...);
		if (namesOfComponents.size()) {
			namesOfComponents.pop_back();
			namesOfComponents.back() = '.';
		}
        SWARN("A group with the same signature already exists. Group consists of: %s", namesOfComponents.c_str());
	}
    return std::dynamic_pointer_cast<ComponentGroup<Component...>>(componentGroups[groupSignature]);
}

template<typename... Component>
std::shared_ptr<ComponentGroup<Component...>> GameWorld::viewGroup() {
	Signature groupSignature = createSignature<Component...>(); 
	if (!componentGroups.count(groupSignature)) {
        std::string namesOfComponents;
		([&] {
            namesOfComponents += typeid(Component).name();
            namesOfComponents += ", ";
		}(), ...);
		if (namesOfComponents.size()) {
			namesOfComponents.pop_back();
			namesOfComponents.back() = '.';
		}
		SERROR("Component group consisting of %s does not exist! Resolved by returning an empty component group.", namesOfComponents.c_str());
		return std::make_shared<ComponentGroup<Component...>>();
	}
	return std::dynamic_pointer_cast<ComponentGroup<Component...>>(componentGroups[groupSignature]);
}

template<typename... Component>
Signature GameWorld::createSignature() {
	Signature signature(0);
	for (int id : {getTypeId<Component>()...})
		signature[id] = true;
    return signature;
}

template<typename Event, typename... DataType>
void GameWorld::broadcastEvent(Event event, DataType... args) {
	systemManager.broadcastEvent(event,shared_from_this(), args...);
}

template<typename Event, typename... DataType>
void GameWorld::deliverEvent(Event event, Entity entity, DataType... args) {
	systemManager.deliverEvent(event, entity, shared_from_this(), args...);
}

} // namespace Saga
