#pragma once
#include <memory>
#include <glm/vec3.hpp>

namespace Saga {

class GameWorld;

namespace Physics {
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
	 * @brief Register all Groups that the Physics meta system use
	 * 
	 * @param world 
	 */
	void registerPhysicsMetaSystem(std::shared_ptr<GameWorld> world);
}

}
