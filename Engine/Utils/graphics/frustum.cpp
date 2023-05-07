#include "frustum.h"
#include "Engine/Datastructures/Accelerant/boundingBox.h"

namespace Saga::Geometry {
    void Frustum::transform(glm::mat4 transformation) {
        for (glm::vec4 &point : allPoints)
            point = transformation * point;
    }

    void Frustum::homogenize() {
        for (glm::vec4 &point : allPoints)
            point /= point.w;
    }

    void Frustum::setFrustum(std::array<glm::vec4, NUM_POINTS> points) {
        allPoints = points;
    }

    Saga::BoundingBox Frustum::getBounds() {
        BoundingBox result = BoundingBox::getExtremeBound();
        for (const glm::vec4 &point : allPoints)
            result = BoundingBox::mergeBox(result, BoundingBox::pointBox(point));
        return result;
    }

    const std::array<glm::vec4, Frustum::NUM_POINTS>& Frustum::getPoints() const {
        return allPoints;
    }
}
