#pragma once

#include "Engine/Components/camera.h"
#include "Engine/Graphics/gaussianBlur.h"
#include "Graphics/GLWrappers/framebuffer.h"
#include "Graphics/GLWrappers/texture.h"
#include <memory>

namespace Saga::Graphics {

/**
 * @brief Handles depth of field calculation and output it to a texture.
 */
class DepthOfField {
public:

    DepthOfField(int width, int height, std::shared_ptr<GraphicsEngine::Texture> resultTex);
    ~DepthOfField();

    void apply(Saga::Camera& camera, 
        std::shared_ptr<GraphicsEngine::Texture> mainTex, std::shared_ptr<GraphicsEngine::Texture> depthTex,
        float focalDistance, float focalRange, int foregroundBlurIterations, int backgroundBlurIterations);

private:
    static int newUID;

    const std::string depthOfFieldShaderID = "depthOfField";
    const std::string depthOfFieldCombineShaderID = "depthOfFieldCombine";

    int uid;
    int width, height;
    std::string splitFBO;
    std::string combineFBO;

    std::shared_ptr<GraphicsEngine::Texture> coc;
    std::shared_ptr<GraphicsEngine::Texture> cocNear;
    std::shared_ptr<GraphicsEngine::Texture> foregroundBlurred[2];
    std::shared_ptr<GraphicsEngine::Texture> backgroundBlurred[2];
    std::shared_ptr<GaussianBlur> foregroundBlur;
    std::shared_ptr<GaussianBlur> backgroundBlur;

    void loadShaderWithParam(Saga::Camera& camera, 
            float focusDistance, float focusRange);
    void generateTextures();
};

}
