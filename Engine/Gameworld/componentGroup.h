#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include "../Entity/entity.h"
#include "gameworld.h"
#include "componentReference.h"

namespace Saga {

/**
 * @brief Generic group of components.
 */
class IComponentGroup {
public:
	/**
	 * @brief Destroy the IComponentGroup object.
	 */
	virtual ~IComponentGroup() = default;
	/**
	 * @brief Handle adding an entity to a group. This entity must have all the components specified by the group
	 * 
	 * @param world 
	 * @param entity 
	 */
	virtual void addEntity(std::shared_ptr<GameWorld> world, const Entity& entity) = 0;

	/**
	 * @brief Handle removing an entity from a group.
	 * 
	 * @param entity 
	 */
	virtual void removeEntity(Entity entity) = 0;
};

/**
 * @brief Effectively a list of tuples each entry containing an Entity and ComponentReference to each of the component type in the group.
 * These Entity must have the specified components, or else there will be unexpected behaviour.
 * When an Entity no longer has all the required components, it must be removed from the Group.
 * 
 * @tparam Component a list of components all entities in the Group shares. This list must be in @em alphabetical order.
 */
template <typename... Component>
class ComponentGroup : public IComponentGroup {
public:
	/**
	 * @brief Destroy the Component Group object.
	 */
    virtual ~ComponentGroup() {}

	/**
	 * @brief Add an Entity to the Group. This entity must contain all the required components. If such an entity already exists in the group, do nothing.
	 * 
	 * @param world 
	 * @param entity 
	 */
	virtual void addEntity(std::shared_ptr<GameWorld> world, const Entity& entity) override;

	/**
	 * @brief Remove an entity from the ComponentGroup.
	 * 
	 * @param entity 
	 */
	virtual void removeEntity(Entity entity) override;

	typename std::vector<std::tuple<Entity, ComponentReference<Component>...>>::iterator begin();
	typename std::vector<std::tuple<Entity, ComponentReference<Component>...>>::iterator end();
    typename std::vector<std::tuple<Entity, ComponentReference<Component>...>>::const_iterator begin() const;
    typename std::vector<std::tuple<Entity, ComponentReference<Component>...>>::const_iterator end() const;
private:
    std::vector<std::tuple<Entity, ComponentReference<Component>...>> allData; //!< list of all entities and references to their components
	std::unordered_map<Entity, size_t> entityToIndex; //!< map between entity and index into allData where the entity lies

	/**
	 * @brief Create an entity-references tuple for a specific entity
	 * 
	 * @param world 
	 * @param entity 
	 * @return std::tuple<Entity, ComponentReference<Component>...> the first component is the entity, and the rest are references to the components specified by the Group, in the order in which they are specified.
	 */
	std::tuple<Entity, ComponentReference<Component>...> createTuple(std::shared_ptr<GameWorld> world, const Entity& entity);
};

} // namespace Saga

#include "componentGroup.inl"
