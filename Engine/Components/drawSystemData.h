#pragma once

#include "Engine/Graphics/gaussianBlur.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/fullscreenquad.h"
#include <memory>

namespace Saga {

struct DrawSystemData {
    // for shadow mapping
    std::shared_ptr<GraphicsEngine::Texture> shadowMap;

    // for post processing
    std::shared_ptr<GraphicsEngine::Texture> screenFragmentColor;
    std::shared_ptr<GraphicsEngine::Texture> extractedBrightColor;
    std::shared_ptr<GraphicsEngine::Texture> extractedBrightColor2;

    std::shared_ptr<Graphics::GaussianBlur> bloomBlur;
};

}
