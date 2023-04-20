#pragma once

#include <glm/vec3.hpp>
#include <optional>

namespace Saga::Geometry {
    /**
     * @brief Find the intersection time t between a ray and a box
     *
     * @param rayOrigin origin of the ray.
     * @param rayDir direction of the ray
     * @param cornerbotLeft bottom left corner of the box.
     * @param cornerTopRIght upper right corner of the box.
     * @return either the time t in [0,1] where rayOrigin + rayDir * t is intersecting the box, or nothing. If the ray starts inside the box, this return 0.
     *
     * @ingroup geometry
     */
    std::optional<float> rayBoxCollision(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 cornerBotLeft, glm::vec3 cornerTopRight);
}
