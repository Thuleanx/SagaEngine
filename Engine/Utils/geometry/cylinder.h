#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <optional>

namespace Saga::Geometry {

    /**
     * @brief Find the minimum translation vector for two axis-aligned cylinders. If the two cylinders are not penetrating, then return the zero vector.
     * 
     * @param height0 height of the first cylinder.
     * @param radius0 radius of the first cylinder.
     * @param pos0 position of the first cylinder.
     * @param height1 height of the second cylinder.
     * @param radius1 radius of the second cylinder.
     * @param pos1 position of the second cylinder.
     * @return glm::vec3 The minimum translation vector.
     * @ingroup geometry
     */
    glm::vec3 detectAACylinderCylinderCollision(float height0, float radius0, glm::vec3 pos0, float height1, float radius1, glm::vec3 pos1);

    /**
     * @brief Find the earliest time between intersection of two cylinders, one of which is moving.
     * It must be the case that the two cylinders are not penetrating at the start. 
     *
     * @param height0 height of the first cylinder.
     * @param radius0 radius of the first cylinder.
     * @param pos0 position of the first cylinder.
     * @param height1 height of the second cylinder.
     * @param radius1 radius of the second cylinder.
     * @param pos1 position of the second cylinder.
     * @param dir the direction of the first cylinder.
     * 
     * @return float t in (0,1] such that pos0 + t * dir is the first time that the two cylinders intersect.
     * @return glm::vec3 normal the normal direction of the contact.
     * @return nothing if the two cylinders are already penetrating, or if t does not exist in the range (0,1].
     * @warn does not work on degenerate cylinders. Will return nothing.
     *
     * @ingroup geometry
     */
    std::optional<std::tuple<float, glm::vec3>> movingCylinderCylinderIntersection(float height0, float radius0, glm::vec3 pos0, float height1, float radius1, glm::vec3 pos1, glm::vec3 dir);
}
