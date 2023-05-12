#include "camera.h"
#include "Engine/Utils/graphics/frustum.h"

namespace Saga {

std::optional<Geometry::Frustum> Camera::getFrustum() {
    if (!camera) return {};

    Geometry::Frustum frustum = Geometry::Frustum();
    std::array<glm::vec4, Geometry::Frustum::NUM_POINTS> pointsOnFrustum;
    int p = 0;
    for (int x = -1; x <= 1; x+=2)
        for (int y = -1; y <= 1; y+=2)
            for (int z = -1; z <= 1; z+=2)
                pointsOnFrustum[p++] = glm::vec4(x, y, z, 1);

    frustum.setFrustum(pointsOnFrustum);

    frustum.transform(glm::inverse(camera->getProjection() * camera->getView()));
    frustum.homogenize();

    return frustum;
}

}
