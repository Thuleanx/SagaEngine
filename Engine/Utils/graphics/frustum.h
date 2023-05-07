#pragma once

#include "Engine/Datastructures/Accelerant/boundingBox.h"
#include <glm/glm.hpp>
#include <array>

namespace Saga::Geometry {
class Frustum {
public:
    static constexpr int NUM_POINTS = 8;
    void setFrustum(std::array<glm::vec4, NUM_POINTS> points);
    void transform(glm::mat4 transformation);
    void homogenize();
    Saga::BoundingBox getBounds();
    const std::array<glm::vec4, NUM_POINTS>& getPoints() const;
private:
    std::array<glm::vec4, NUM_POINTS> allPoints;
};
}

