#pragma once

#include "Engine/Components/camera.h"
#include <memory>
#include <optional>

namespace Saga {
    class GameWorld;
}

namespace Saga::Systems::Graphics {
    void shadowMapSetup(std::shared_ptr<Saga::GameWorld> world);
    std::optional<glm::mat4> renderShadowMap(std::shared_ptr<Saga::GameWorld> world, Saga::Camera& camera);
    void drawShadowMapGizmos();
}
