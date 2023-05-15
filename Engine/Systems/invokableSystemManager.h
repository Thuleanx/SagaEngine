#pragma once
#include "systemManager.h"
#include "../Entity/entity.h"

namespace Saga {

class GameWorld;

/**
 * @brief A SystemManager where stages and events can be invoked.
 */
class InvokableSystemManager : public SystemManager {
public:
	/**
	 * @brief Invoke the startup stage. This calls all systems attached in the Awake and Start stages.
	 * @param gameWorld 
	 */
	void runStageStartup(std::shared_ptr<GameWorld> gameWorld);

	/**
	 * @brief Invoke the update stage. This calls any PreUpdate, Update, and LateUpdate systems in that order.
	 * 
	 * @param gameWorld 
	 * @param deltaTime the time since the last update.
	 * @param time total time ellapsed since the start of the program.
	 */
	void runStageUpdate(std::shared_ptr<GameWorld> gameWorld, float deltaTime, float time);

	/**
	 * @brief Invoke the fixed update stage. This calls any FixUpdate, and LateFixedUpdate systems in that order.
	 * 
	 * @param gameWorld 
	 * @param deltaTime the time since the last fixed update.
	 * @param time total time ellapsed since the start of the program.
	 */
	void runStageFixedUpdate(std::shared_ptr<GameWorld> gameWorld, float deltaTime, float time);

	/**
	 * @brief Invoke the Draw stage.
	 * 
	 * @param gameWorld 
	 */
	void runStageDraw(std::shared_ptr<GameWorld> gameWorld);

	/**
	 * @brief Invoke the Cleanup stage.
	 * 
	 * @param gameWorld 
	 */
	void runStageCleanup(std::shared_ptr<GameWorld> gameWorld);

	/**
	 * @brief Broadcast an event. Only systems registered that are not tied to a specific entity will receive this broadcast.
	 * 
	 * @tparam Event anything castable to an int.
	 * @tparam DataType the type of arguments that the Systems listening to this event will receive.
	 * @param event 
	 * @param gameWorld 
	 * @param args arguments to be passed to the systems.
	 */
	template <typename Event, typename ...DataType>
	void broadcastEvent(Event event, std::shared_ptr<GameWorld> gameWorld, DataType... args) {
		if (eventSystemsMap.count((int) event))
			eventSystemsMap[(int) event].invoke(-1, gameWorld, args...);
	}

	/**
	 * @brief Deliver an event to an entity. Any system attached to that entity with this event will receive this delivery.
	 * 
	 * @tparam Event anything castable to an int.
	 * @tparam DataType the type of arguments that the systems listening to this event will receive.
	 * @param event 
	 * @param entity the entity to deliver the event to.
	 * @param gameWorld 
	 * @param args arguments to be passed to the systems attached to this event.
	 */
	template <typename Event, typename ...DataType>
	void deliverEvent(Event event, Entity entity, std::shared_ptr<GameWorld> gameWorld, DataType... args) {
		if (eventSystemsMap.count((int) event)) {
            eventSystemsMap[(int) event];
            STRACE("testing event: %d, entity: %d", (int) event, (int) entity);
  	        eventSystemsMap[(int) event].invoke(entity,
                gameWorld, entity, args...);
        }
	}

	/**
	 * @brief Broadcast a key event.
	 * 
	 * @param gameWorld 
	 * @param key a GLFW key.
	 * @param action a GLFW action, like GLFW_PRESSED or GLFW_RELEASE.
	 */
    void keyEvent(std::shared_ptr<GameWorld> gameWorld, int key, int action);

	/**
	 * @brief Broadcast a mouse position event.
	 * 
	 * @param gameWorld 
	 * @param xpos the x position of the mouse in screen space.
	 * @param ypos the y position of the mouse in screen space.
	 */
    void mousePosEvent(std::shared_ptr<GameWorld> gameWorld, double xpos, double ypos);

	/**
	 * @brief Broadcast a mouse button event.
	 * 
	 * @param gameWorld 
	 * @param key a GLFW key.
	 * @param action a GLFW action, like GLFW_PRESSED or GLFW_RELEASE.
	 */
    void mouseButtonEvent(std::shared_ptr<GameWorld> gameWorld, int button, int action);

	/**
	 * @brief Broadcast a scroll event.
	 * 
	 * @param gameWorld 
	 * @param distance amount of pixels scrolled.
	 */
    void scrollEvent(std::shared_ptr<GameWorld> gameWorld, double distance);

	/**
	 * @brief Broadcast a window resize event.
	 * 
	 * @param gameWorld 
	 * @param width the width in pixels of the new window.
	 * @param height the height in pixels of the new window.
	 */
	void windowResizeEvent(std::shared_ptr<GameWorld> gameWorld, int width, int height);

    /**
     * @brief Called when an entity is destroyed.
     * This disconnects all events that is related to this entity.
     */
    void onEntityDestroyed(Entity entity);

};
}
