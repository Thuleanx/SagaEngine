#pragma once

#include "Engine/Datastructures/Accelerant/boundingBox.h"
#include <glm/glm.hpp>
#include <array>

namespace 
class Frustum {
public:
    static constexpr int NUM_POINTS = 8;
    void transform(glm::mat4 transformation);
    Saga::BoundingBox getBounds();
private:
    std::array<glm::vec4, NUM_POINTS> allPoints;
};
