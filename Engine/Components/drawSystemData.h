#pragma once

#include "Graphics/GLWrappers/texture.h"
#include <memory>

namespace Saga {

struct DrawSystemData {
    std::shared_ptr<GraphicsEngine::Texture> shadowMap;
};

}
