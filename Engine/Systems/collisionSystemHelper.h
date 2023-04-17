#pragma once

#include "Engine/Components/collisionSystemData.h"
#include "Engine/Systems/collisionSystem.h"
#include "glm/ext/vector_float3.hpp"
#include <optional>

namespace Saga::Systems {
    /**
     * @brief Retrieve the closest collision to a moving object.
     *
     * @param world
     * @param systemData the data for doing collision detection. This value is optional, and 
     *      if not specified, the function retrieves it from world.
     * @param entity the moving entity.
     * #param cylinderCollider the cylinder collider of the moving entity.
     * @param pos the starting position of the moving ellipsoid.
     * @param dir the direction (as well as distance) the ellipsoid moves.
     *
     * @return Collision at time t in [0,1] where position + t * dir is where the ellipsoid first collides with another object.
     * @return nothing if no collision exists.
     */
    std::optional<Collision> getClosestCollision(
        std::shared_ptr<GameWorld> world, std::optional<CollisionSystemData*> systemData,
        Entity entity, EllipsoidCollider& ellipsoidCollider, 
        std::optional<CylinderCollider*> cylinderCollider, glm::vec3 pos, glm::vec3 dir);

    /**
     * @brief Nudge the ellipsoid along the direction specified by the collision normal.
     * This prevents the ellipsoid from sliding along a surface and thus detecting numerous collisions.
     *
     * @param pos the current position of the ellipsoid.
     * @param collision the collision used to nudge this ellipsoid.
     *
     * @return glm::vec3 the position of the ellipsoid after nudging.
     */
    glm::vec3 doNudge(
        std::shared_ptr<GameWorld> world, std::optional<CollisionSystemData*> systemData,
        Entity entity, EllipsoidCollider& ellipsoidCollider, 
        std::optional<CylinderCollider*> cylinderCollider,
        glm::vec3 pos, Collision &collision);
}
