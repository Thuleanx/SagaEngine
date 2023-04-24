#pragma once

#include <optional>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../Entity/entity.h"

namespace Saga {

/**
 * @brief Generic container of components.
 */
class IComponentContainer {
public:
	/**
	 * @brief Destroy the IComponentContainer object
	 */
	virtual ~IComponentContainer() = default;

	/**
	 * @brief Handler for when an entity is deleted. This should remove the entity from the Container, as well as the associated component if it exists.
	 * @param entity 
	 */
	virtual void onEntityDestroyed(Entity entity) = 0;

	/**
	 * @brief Get the last time the container was changed. Useful for cacheing references from the container
	 * @return int time step at which the container has been changed.
	 */
	virtual int getLastReallocated() = 0;
};

/**
 * @brief Container for a specific Component type.
 * 
 * @tparam Component the component this container manages.
 */
template <typename Component>
class ComponentContainer : public IComponentContainer {
public:
	/**
	 * @brief Destroy the Component Container object
	 * 
	 */
	virtual ~ComponentContainer() {}

	/**
	 * @brief Emplace a component onto an entity. This created
	 * 
	 * @tparam Args the argument types that the component's constructor accepts.
	 * @param entity the entity to add the component into.
	 * @param args the arguments used to construct the component.
	 * @return Component* a pointer to the component.
	 * @throw std::invalid_argument if the entity already has this component attached.
	 */
	template <typename... Args>
	Component* emplace(const Entity entity, Args &&...args);

	typename std::vector<Component>::iterator begin();
	typename std::vector<Component>::iterator end();
    typename std::vector<Component>::const_iterator begin() const;
    typename std::vector<Component>::const_iterator end() const;

    /**
     * @brief Retrieve the size of the container.
     */
    std::size_t size() const { return std::distance(begin(), end()); }


	/**
	 * @brief Get a pointer to a Component attached to an entity.
	 * 
	 * @param entity 
	 * @return Component* the Component attached to the entity. nullptr if none exists.
	 */
	Component* getComponent(const Entity entity);

    /**
     * @brief Return any component in the container.
     * 
     * @return Component* a pointer to said component if possible.
     * @return nothing if this container is empty.
     */
    std::optional<Component*> any();

	/**
	 * @brief Determine if an entity has this type of Component attached.
	 * 
	 * @param entity 
	 * @return true if the entity has the component.
	 * @return false otherwise.
	 */
	bool hasComponent(const Entity entity);

	/**
	 * @brief Get an Entity with this specific component.
	 * @warning runs in O(n).
	 * 
	 * @param component 
	 * @return Entity the entity the component belongs to, or -1 if none exists.
	 */
	Entity getEntity(Component* component);

	/**
	 * @brief Remove a component from an entity. 
	 * This also allows the slot used for the component to be free to used for when new components gets added.
	 * 
	 * @param entity 
	 */
	void removeComponent(const Entity entity);

	/**
	 * @return int number of active components in this container.
	 */
	inline int getActiveCnt() { return cnt; } 

	/**
	 * @brief Get the last time the component container resize the vector containing all the components.
	 * 
	 * @return int an increment value that shows when the last reallocated time is. It starts at 0 and increase by 1 every time a rellocation happens. 
	 * When reallocation happens, all pointers to previous components drop.
	 */
	int getLastReallocated() override { return lastReallocated; }
private:
	// lets goo
	std::vector<Component> components;
	std::unordered_map<Entity, size_t> componentMap; //!< map between Entity and which index its component is located at.
	std::unordered_map<size_t, Entity> entityMap; //!< map between index and which entity the component at that index belongs to.
	int cnt = 0; //!< number of active components
	int lastReallocated = 0; //!< time at which the last reallocation happens

	void onEntityDestroyed(Entity entity) override;
	/**
	 * @brief Readjust the size of the component vector. This should not affect any of the maps, and should have amortized O(1) performance.
	 */
	void tryRepack();
};

} // namespace Saga

#include "componentContainer.inl"
