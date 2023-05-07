#include "gaussianBlur.h"
#include "Engine/Graphics/blit.h"
#include "Engine/Graphics/separableKernel.h"
#include "Engine/_Core/logger.h"
#include "Graphics/GLWrappers/framebuffer.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/global.h"

namespace Saga::Graphics {

GaussianBlur::GaussianBlur(int width, int height, std::shared_ptr<GraphicsEngine::Texture> texture0, std::shared_ptr<GraphicsEngine::Texture> texture1, float sigma) :
    SeparableKernel(width, height, texture0, texture1), sigma(sigma) {

    if (sigma <= 0) {
        SERROR("Sigma used for gaussian blur is non possitive. This will cause unexpected behaviour.");
    }

    constructShader();
    GraphicsEngine::Global::graphics.bindShader(getShader());
    initializeShaderParams();
}

GaussianBlur::~GaussianBlur() {
    GraphicsEngine::Global::graphics.removeShader(getShader());
}

std::string GaussianBlur::getShader() { return blurShaderId + std::to_string(uid); }

void GaussianBlur::constructShader() {
    using namespace GraphicsEngine::Global;
    graphics.addShader(getShader(), {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/standard/gaussianBlur.frag"});
}

void GaussianBlur::initializeShaderParams() {
    using namespace GraphicsEngine::Global;
    auto blurShader = graphics.getActiveShader();

    float normalizingFactor = 1/std::sqrt(2 * M_PI * sigma  * sigma);
    for (int i = 0; i < KERNEL_SIZE; i++) {
        std::string prop = "weight[" + std::to_string(i) + "]";
        float value = normalizingFactor * std::exp((-i*i) / (2 * sigma * sigma));
        blurShader->setFloat(prop, value);
    }
    // beyond 3*sigma, the value is effectively 0
    blurShader->setInt("kernelSz", std::min(KERNEL_SIZE, (int) std::floor(3 * sigma + 1)));

    graphics.getActiveShader()->setSampler("MainTex", 0);
}

}
