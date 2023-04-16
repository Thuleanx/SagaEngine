#pragma once
#include "Engine/Components/collider.h"
#include <memory>
#include <glm/vec3.hpp>
#include <optional>

namespace Saga {

class GameWorld;

namespace Physics {
    struct RaycastHit {
        Collider* collider;
        float t;
        glm::vec3 pos;
        glm::vec3 normal;
    };

	/**
	 * @brief Determine if a cylinder overlaps with another cylinder in the world.
	 * 
	 * @param world 
	 * @param height height of the cylinder.
	 * @param radius radius of the cylinder.
	 * @param pos position of the cylinder.
	 * @return true if the cylinder overlaps with another cylinder collider.
	 * @return false otherwise.
	 */
	bool overlapCylinder(std::shared_ptr<GameWorld> world, float height, float radius, glm::vec3 pos);

    /**
     * @brief Cast a ray, hitting any mesh collider.
     *
     * @param world
     * @param pos the starting position of the raycast in world space
     * @param dir the direction of the raycast in world space
     */
    std::optional<RaycastHit> raycastAllTriangles(std::shared_ptr<GameWorld> world, glm::vec3 pos, glm::vec3 dir);

	/**
	 * @brief Register all Groups that the Physics meta system use
	 * 
	 * @param world 
	 */
	void registerPhysicsMetaSystem(std::shared_ptr<GameWorld> world);
}

}
