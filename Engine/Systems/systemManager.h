#pragma once

#include "system.h"
#include <atomic>
#include <functional>
#include <memory>
#include <unordered_map>
#include "../Datastructures/eventmap.h"
#include "../Entity/entity.h"

namespace Saga {

/**
 * @brief Manages Systems in the ECS model. Allows for System to be added, but not invoked.
 * 
 */
class SystemManager {
public:
	/**
	 * @brief Stages in the Saga engine where systems can be set to run in.
	 * Awake is a stage that happens at the very start of an entity.
	 * Start happens on the firsts frame. Every frame thereafter, PreUpdate, Update, and LateUpdate is called.
	 * FixedUpdate is called on every physics frame, and so is LateFixedUpdate.
	 * The Draw stage happens at the end of every frame, where we have to render the output to the screen.
	 */
	enum class Stage {
		Awake,
		Start,
		PreUpdate,
		Update,
		LateUpdate,
		FixedUpdate,
		LateFixedUpdate,
		Draw,
		Cleanup
	};

	/**
	 * @brief Used for id of Systems.
	 */
	enum Id : uint64_t {};

public:
	SystemManager();
	virtual ~SystemManager();

	/**
	 * @brief Add a staged System to the list of systems.
	 * 
	 * @tparam DataType the data types that the System accepts. 
	 * @param system 
	 * @param stage which stage is the System attached to. When this stage is invoked, all systems attached to the stage is invoked.
	 * @return EventMap::Id id of the System, can be used to remove the system later.
	 */
	template <typename ...DataType>
	EventMap::Id addStagedSystem(System<DataType...> system, Stage stage = Stage::Update);

	/**
	 * @brief Remove a staged System.
	 * 
	 * @param id the id of the system to be removed.
	 * @param stage the stageSof the System. 
	 * @see Stage
	 */
	void removeStagedSystem(EventMap::Id id, Stage stage = Stage::Update);

	/**
	 * @brief Add an event System. 
	 * 	Event systems are Systems triggerable by events broadcasted by the program, and not neccessarily the stages in normal execution.
	 * 	These Systems only respond to when events are being broadcasted.
	 * 
	 * @tparam Event could be anything castable to an integer.
	 * @tparam DataType the data types that the Systems that receive these events accept.
	 * @param event the event.
	 * @param system the System to add.
	 * @return EventMap::Id id of the System, can be used to remove the system later
	 */
	template <typename Event, typename ...DataType>
	EventMap::Id addEventSystem(Event event, System<DataType...> system);

	/**
	 * @brief Remove the System from being called when an event is broadcasted.
	 * 
	 * @tparam Event can be anything castable to an integer.
	 * @param event the event.
	 * @param id the id of the System to be removed.
	 */
	template <typename Event>
	void removeEventSystem(Event event, EventMap::Id id);

	/**
	 * @brief Add an event System specific to an entity.
	 * This event System will be triggered when that event is delivered to the entity.
	 * 
	 * @tparam Event can be anything castable to an integer.
	 * @tparam DataType the data type the System accepts as argument.
	 * @param event 
	 * @param entity 
	 * @param system 
	 * @return EventMap::Id id of the System, can be used to remove it later.
	 */
	template <typename Event, typename ...DataType>
	EventMap::Id addEventSystem(Event event, Saga::Entity entity, std::shared_ptr<System<Saga::Entity, DataType...>> system);

	/**
	 * @brief Remove the System from being called when an event is delivered.
	 * 
	 * @tparam Event can be anything castable to an int.
	 * @param event 
	 * @param entity 
	 * @param id 
	 */
	template <typename Event>
	void removeEventSystem(Event event, Saga::Entity entity, EventMap::Id id);

	/**
	 * @brief Add a System to respond to a keyboard input.
	 * 
	 * @param key key as an GLFW enum.
	 * @param system System to receive the key input.
	 * @return EventMap::Id id of the event, can be used to remove it later.
	 */
	EventMap::Id addKeyboardEventSystem(int key, System<int> system);

	/**
	 * @brief Remove a keyboard System.
	 * 
	 * @param key 
	 * @param id 
	 */
	void removeKeyboardEventSystem(int key, EventMap::Id id);

	/**
	 * @brief Add a System to respond to a mouse button inputs.
	 * 
	 * @param key key as an GLFW enum.
	 * @param system 
	 * @return EventMap::Id 
	 */
	EventMap::Id addMouseEventSystem(int key, System<int> system);

	/**
	 * @brief Remove a System from responding to mouse button inputs.
	 * 
	 * @param key key as an GLFW enum.
	 * @param id 
	 */
	void removeMouseEventSystem(int key, EventMap::Id id);

	/**
	 * @brief Add a System to receive mouse position
	 * 
	 * @param system must accept <double, double> as the position of the mouse in screen space.
	 * @return EventMap::Id 
	 */
	EventMap::Id addMousePosSystem(System<double, double> system);
	void removeMousePosSystem(EventMap::Id id);

	/**
	 * @brief Add a System to receive scroll input.
	 * 
	 * @param system must accept <double> as the amount of pixels scrolled.
	 * @return EventMap::Id 
	 */
	EventMap::Id addScrollSystem(System<double> system);
	void removeScrollSystem(EventMap::Id id);

	/**
	 * @brief Add a System to receive when the window resize.
	 * 
	 * @param system must accept <int,int> as the dimensions of the window after resized.
	 * @return EventMap::Id 
	 */
	EventMap::Id addWindowResizeSystem(System<int, int> system);

	/**
	 * @brief Remove a resize System.
	 * 
	 * @param id 
	 */
	void removeWindowResizeSystem(EventMap::Id id);

protected:
	/**
	 * @brief Types of input events, with button events excluded.
	 * 
	 */
	enum class OtherInput {
		MOUSE_POS,
		SCROLL,
		WINDOW_RESIZE
	};
	static auto id_value()
		-> uint64_t & {
		static uint64_t the_id;
		return the_id;
	}

	/**
	 * @brief Map between Stage and Systems
	 */
	EventMap stagedSystemsMap;

	/**
	 * @brief Maps entity to an EventMap that maps Event to System
	 */
	std::unordered_map<int, EventMap> eventSystemsMap;

	// input maps
	EventMap keyboardInputMap;
	EventMap mouseInputMap;
	EventMap otherInputMap; //< scroll and mouse pos falls into this category
};
} // namespace Saga

#include "systemManager.inl"
