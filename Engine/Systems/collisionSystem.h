#pragma once

#include <memory>
#include "Engine/saga.h"
#include <glm/vec3.hpp>

namespace Saga {
	class GameWorld;
}

namespace Saga::Systems {

	/**
	 * @brief Collision struct that details a collision.
	 */
	struct Collision {
		float t; //!< time at which the collision happens. -1 if this collision object is not a collision.
		glm::vec3 pos; //!< position of the collision.
		glm::vec3 normal; //!< normal direction of the collision.
		Entity entity0; //!< first Entity in the collision.
		Entity entity1; //!< second Entity in the collision. The normal direction is away from this Entity.

		Collision() { t = -1; }
		Collision(float t, glm::vec3 pos, glm::vec3 normal, Entity e1, Entity e2) : 
			t(t), pos(pos), normal(normal), entity0(e1), entity1(e2) {}
	};

	/**
	 * @brief System for handling collisions.
	 *
	 * @ingroup system
	 * @param world 
	 * @param deltaTime time since last call of this System.
	 * @param time time ellapsed since the start of the game.
	 */
	void collisionSystem(std::shared_ptr<GameWorld> world, float deltaTime, float time);


    /**
     * @brief System for handling collision. This should be run on startup / awake,
     * and is responsible for constructing the datastructure neccessary for 
     * collision detection
     *
     * @param world
     */
    void collisionSystem_startup(std::shared_ptr<GameWorld> world);

	/**
	 * @brief Register all Group for this System, as well as register
     * collisionSystem and collisionSystem as Update and Awake staged systems
     * respectively.
	 * 
	 * @param world 
	 */
	void registerCollisionSystem(std::shared_ptr<GameWorld> world);
}
