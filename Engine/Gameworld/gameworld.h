#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../Datastructures/typemap.h"
#include "../Systems/invokableSystemManager.h"
#include "../Systems/system.h"
#include "../Entity/entity.h"
#include "signature.h"
#include "componentReference.h"

namespace Saga {

class IComponentContainer;

template<typename Component>
class ComponentContainer;

class IComponentGroup;

template<typename... Component>
class ComponentGroup;

/**
 * @brief Maintains a game world, where Entity, Component, Systems, and MetaSystems can be added, and events such as Startup, Update, FixedUpdate, and Draw can be invoked.
 */
class GameWorld : public std::enable_shared_from_this<GameWorld> {
public: 
	/**
	 * @brief Construct a new Game World object.
	 */
	GameWorld();
	/**
	 * @brief Destroy the Game World object.
	 */
	virtual ~GameWorld();

	/**
	 * @brief Create a Entity object.
	 * 
	 * @return Entity 
	 */
	Entity createEntity();

    /**
     * @brief Get the master entity. Use this entity only for placing global data, such as 
     * baked navmeshes, lightning information, etc. Do not use it as a gameobject.
     *
     * @return Enity the master entity. Every gameworld will have one created at the start.
     */
    Entity getMasterEntity();

	/**
	 * @brief Destoy an Entity object. This does not immediately destroy said entity, but wait until all systems are done with processing
     * all entities to destroy it.
	 * 
	 * @param entity 
	 */
	void destroyEntity(Entity entity);

	/**
	 * @brief Emplace a component to an Entity. This constructs the component instead of adding them.
	 * 
	 * @tparam Component The type of the Component.
	 * @tparam Args The type of the parameters used to construct the Component.
	 * @param entity 
	 * @param args the arguments used the construct the Component
	 * @return ComponentReference<Component> a reference to the Component.
	 * @throw std::invalid_argument if the entity already has that Component. For this reason, it is good to check give if the component exists.
	 */
    template<typename Component, typename ...Args>
	ComponentReference<Component> emplace(const Entity entity, Args &&...args);

	/**
	 * @brief Get a reference to a Component on the specified entity.
	 * 
	 * @tparam Component the type of the component to look for.
	 * @param entity 
	 * @return ComponentReference<Component> a reference to that component. This reference can be a non-reference, which evaluates to nullptr.
	 */
    template<typename Component>
	ComponentReference<Component> getComponent(const Entity entity);

	// slow af so don't use
	/**
	 * @brief Get the Entity from a pointer to a component.
	 * 
	 * @tparam Component the type of component you use.
	 * @param component 
	 * @return Entity the entity, or -1 if none exists.
	 * @warning this runs in O(n), where n is the number of components of that type.
	 */
    template<typename Component>
	Entity getEntity(Component* component);

	/**
	 * @brief Determine if an entity has a specific type of Component attached.
	 * 
	 * @tparam Component the type of the Component.
	 * @param entity 
	 * @return true if it does.
	 * @return false otherwise.
	 */
    template<typename Component>
	bool hasComponent(const Entity entity);

	/**
	 * @brief Remove a component from an Entity.
	 * 
	 * @tparam Component type of the component.
	 * @param entity 
	 */
    template<typename Component>
	void removeComponent(const Entity entity);

	/**
	 * @tparam Component type of component to look for.
	 * @return std::shared_ptr<ComponentContainer<Component>> a container of components that can be iterated through. 
	 */
	template<typename Component>
	std::shared_ptr<ComponentContainer<Component>> viewAll();

	/**
	 * @brief Register a ComponentGroup.
	 * 
	 * @tparam Component a list of components entities in the group must have, in @em alphabetical order. 
	 * @return std::shared_ptr<ComponentGroup<Component...>> a shared_ptr to that group.
	 */
	template<typename... Component>
	std::shared_ptr<ComponentGroup<Component...>> registerGroup();

	/**
	 * @brief Compute the view of a Group. It is mandatory that this group is registered before this operation, and before any components were added to the world.
	 * 
	 * @tparam Component the types of components the group has, in @em alphabetical order.
	 * @return std::shared_ptr<ComponentGroup<Component...>> a group that can be iterated through, containing all the entities with the specified group.
	 */
	template<typename... Component>
	std::shared_ptr<ComponentGroup<Component...>> viewGroup();

	/**
	 * @brief Get the SystemManager object.
	 * 
	 * @return SystemManager& 
	 */
    SystemManager& getSystems() { return systemManager; }

	/**
	 * @brief Get an id for a type, useful for keeping multiple typemaps consistent.
	 * 
	 * @tparam KeyType the type.
	 * @return int 
	 */
	template <class KeyType>
	int getTypeId() { return componentMap.getTypeId<KeyType>(); }

	/**
	 * @brief Broadcast an event to the world, so that Systems that listen to the event can be triggered.
	 * 
	 * @tparam Event can be anything castable to an int.
	 * @tparam DataType the type of parameters used to broadcast the event, and also the types that the Systems will receive.
	 * @param event 
	 * @param args arguments for the Systems that are listening to this event.
	 */
	template<typename Event, typename... DataType>
	void broadcastEvent(Event event, DataType... args);

	/**
	 * @brief Deliver an event to a specific object. Only System that registers to that specific object will be called.
	 * 
	 * @tparam Event anything castable to an int.
	 * @tparam DataType the type of parameters used in the delivery.
	 * @param event 
	 * @param entity the entity to deliver the event to.
	 * @param args arguments for the Systems that are listening to this event.
	 */
	template<typename Event, typename... DataType>
	void deliverEvent(Event event, Entity entity, DataType... args);

protected:
	entity_type entity_cnt = 1; //!< number of entities in the world. Useful for assigning new entities. We start at 1, because we assume an entity 0 where the engine can place game information on.

	/**
	 * @brief Create a signature from a list of component types.
	 * 
	 * @tparam Component a list of component types, in @em alphabetical order.
	 * @return Signature the correponding signature.
	 */
	template<typename... Component>
	Signature createSignature();

	InvokableSystemManager systemManager; //!< where all the systems are stored and can potentially be invoked.

    /**
     * @brief Destroy / cleanup any entity. This should happen after every frame, so that all entities that
     * is signaled to be destroyed will be cleaned up. This ensures entities are not deleted while a
     * system is still processing it.
     */
    void entityCleanup();
private:
	TypeMap<std::shared_ptr<IComponentContainer>> componentMap; //!< map between Component and their containers
	std::unordered_map<Entity, Signature> entitySignatures; //!< map between entities and their signature
	std::unordered_map<Signature, std::shared_ptr<IComponentGroup>> componentGroups; //!< map between signature and ComponentGroup
    std::unordered_set<Entity> entitiesToDestroy;


};

} // namespace Saga

#include "gameworld.inl"
