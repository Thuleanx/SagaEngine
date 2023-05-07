#include "camera.h"

namespace Saga {

std::optional<Geometry::Frustum> Camera::getFrustum() {
    if (!camera) return {};

    Geometry::Frustum frustum = Geometry::Frustum();
    frustum.setFrustum({
        glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
        glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),
        glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
        glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),
        glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f)
    });

    frustum.transform(glm::inverse(camera->getProjection() * camera->getView()));
    frustum.homogenize();

    return frustum;
}

}
