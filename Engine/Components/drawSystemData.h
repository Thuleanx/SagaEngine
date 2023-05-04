#pragma once

#include "Engine/Graphics/fog.h"
#include "Engine/Graphics/gaussianBlur.h"
#include "Engine/Systems/helpers/postProcessing.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/fullscreenquad.h"
#include <memory>

namespace Saga {

struct DrawSystemData {
    // for shadow mapping
    std::shared_ptr<GraphicsEngine::Texture> shadowMap;

    // for post processing
    std::shared_ptr<GraphicsEngine::Texture> screenFragmentColor;
    std::shared_ptr<GraphicsEngine::Texture> screenFragmentColorAfterFog;
    std::shared_ptr<GraphicsEngine::Texture> depthStencil;
    std::shared_ptr<GraphicsEngine::Texture> bloomColor0;
    std::shared_ptr<GraphicsEngine::Texture> bloomColor1;

    std::shared_ptr<Graphics::GaussianBlur> bloomBlur;
    std::shared_ptr<Graphics::Fog> fog;

    Systems::Graphics::PostProcessingSettings postProcessingSettings;
};

}
