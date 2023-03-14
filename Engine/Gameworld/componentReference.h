#pragma once

#include <memory>
#include "../Entity/entity.h"
#include "componentContainer.h"
#include <iostream>

namespace Saga {

/**
 * @brief Reference to a component. Will be valid as long as that component exists on a specific Entity, even if their pointer address has changed.
 * 
 * @tparam Component type of component this reference is for.
 */
template <typename Component>
class ComponentReference {
public:
	/**
	 * @brief Construct a new Component Reference object.
	 */
	ComponentReference() {}
	/**
	 * @brief Construct a new Component Reference object.
	 * 
	 * @param componentContainer 
	 * @param entity 
	 */
	ComponentReference(std::shared_ptr<ComponentContainer<Component>> componentContainer, Entity entity) 
	: componentContainer(componentContainer), entity(entity) {}

	/**
	 * @brief Destroy the Component Reference object.
	 */
	virtual ~ComponentReference() {};

	/**
	 * @brief Get a volatile pointer to the component this references. 
	 * Since this pointer can change when components of the same type are added and removed from the world, it is not a good idea to use this pointer for a long time.
	 * 
	 * @return Component* a reference to the component, or nullptr if none exists.
	 */
    operator Component*() { return getVolatile(); }

	/**
	 * @brief Automatic cast of ComponentReference to their volatile Component pointer form.
	 * 
	 * @return Component* pointer to the component that this object references, or nullptr if they don't exist.
	 */
    Component* operator->() { return getVolatile(); }

private:
	// will be wonky behaviour if you encounter the ABA problem. But if you do it's not really the engine's fault is it
	int lastReallocated = -1;
	Component* cachedComponent = nullptr; //!< a cached pointer to the component. Will be valid as long as the container has not reallocated its memory.
	std::shared_ptr<ComponentContainer<Component>> componentContainer;
	Entity entity;

	/**
	 * @brief Get a volatile pointer to the component this references.
	 * 
	 * @return Component* pointer to the component, nullptr if it does not exist.
	 */
    Component* getVolatile() {
		if (!componentContainer) return nullptr;
		// if reallocated, we need to fetch a new cache
        if (componentContainer->getLastReallocated() > lastReallocated) {
            cachedComponent = componentContainer->getComponent(entity);
            lastReallocated = componentContainer->getLastReallocated();
		}
		return cachedComponent;
	}
};

} // namespace Saga

