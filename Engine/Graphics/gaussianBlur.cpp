#include "gaussianBlur.h"
#include "Engine/Graphics/blit.h"
#include "Engine/_Core/logger.h"
#include "Graphics/GLWrappers/framebuffer.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/global.h"

namespace Saga::Graphics {

int GaussianBlur::newUID = 0;

GaussianBlur::GaussianBlur(int width, int height, std::shared_ptr<GraphicsEngine::Texture> texture0, std::shared_ptr<GraphicsEngine::Texture> texture1, float sigma) :
    uid(newUID++), width(width), height(height), sigma(sigma) {

    if (sigma <= 0) {
        SERROR("Sigma used for gaussian blur is non possitive. This will cause unexpected behaviour.");
    }

    textures[0] = texture0;
    textures[1] = texture1;

    for (int i = 0; i < 2; i++) {
        using namespace GraphicsEngine::Global;
        fbo[i] = "gaussianBlurFramebuffer_" + std::to_string(uid) + "_" + std::to_string(i);
        graphics.addFramebuffer(fbo[i], width, height);
        graphics.getFramebuffer(fbo[i])->attachTexture(textures[i^1], GL_COLOR_ATTACHMENT0);
        graphics.getFramebuffer(fbo[i])->verifyStatus();
    }

    using namespace GraphicsEngine::Global;

    graphics.addShader(blurShaderId, {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/standard/gaussianBlur.frag"});

    loadShaderWithParam();
}

GaussianBlur::~GaussianBlur() { 
    using namespace GraphicsEngine::Global;
    graphics.removeFramebuffer(fbo[0]);
    graphics.removeFramebuffer(fbo[1]);
}

void GaussianBlur::loadShaderWithParam() {
    using namespace GraphicsEngine::Global;

    graphics.bindShader(blurShaderId);

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

void GaussianBlur::blur(int iterations) {
    while (iterations-->0) {
        using namespace GraphicsEngine::Global;

        graphics.bindShader(blurShaderId);
        auto blurShader = graphics.getActiveShader();
        for (int horizontal = 0; horizontal <= 1; horizontal++) {
            blurShader->setInt("horizontal", horizontal);
            blit(fbo[horizontal], blurShaderId, textures[horizontal]);
        }
    }
}

}
