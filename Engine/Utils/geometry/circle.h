#pragma once

#include <glm/vec2.hpp>
#include <optional>

namespace Saga::Geometry {
    /**
     * @brief Detect if two circles overlap. If they do, output the minimum translation vector from the first circle for both to not be colliding.
     * 
     * @param aPos center of the first circle.
     * @param aRadius radius of the first circle.
     * @param bPos center of the second circle.
     * @param bRadius radius of the second circle.
     * @return glm::vec2 zero if non overlapping. The minimum translation vector with respect to the first circle otherwise.
     */
    glm::vec2 detectCircleCollision(const glm::vec2& aPos, const float &aRadius, const glm::vec2& bPos, const float &bRadius);

    /**
     * @brief Find the entrance and exit times t between a unit circle at the origin and a ray.
     * 
     * @param origin origin of the ray.
     * @param rayDirection direction of the ray.
     * @return tlo <= thi such that origin + tlo * rayDirection is the intersection between the ray and the unit circle, 
     *  and the same applies to thi.
     * @return nothing if no intersection exists.
     */
    std::optional<std::tuple<float,float>> rayUnitCircleAtOriginIntersection(const glm::vec2& origin, const glm::vec2& rayDirection);
}
