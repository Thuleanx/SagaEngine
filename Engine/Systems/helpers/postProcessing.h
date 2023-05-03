#pragma once

#include "Engine/Components/camera.h"
#include <memory>

namespace Saga {
    class GameWorld;
}

namespace Saga::Systems::Graphics {
    void drawPostProcessingGizmos();
    void postProcessingSetup(std::shared_ptr<Saga::GameWorld> world, Saga::Camera& camera);
    void usePostProcessingFBO();
    void performPostProcessing(std::shared_ptr<Saga::GameWorld> world, Saga::Camera& camera);
}
