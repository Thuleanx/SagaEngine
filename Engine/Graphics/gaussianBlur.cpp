#include "gaussianBlur.h"
#include "Engine/_Core/logger.h"
#include "Graphics/GLWrappers/framebuffer.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/fullscreenquad.h"
#include "Graphics/global.h"

namespace Saga::Graphics {

GaussianBlur::GaussianBlur(int width, int height, std::shared_ptr<GraphicsEngine::Texture> texture0, std::shared_ptr<GraphicsEngine::Texture> texture1, float sigma) :
    width(width), height(height), sigma(sigma) {

    if (sigma <= 0) {
        SERROR("Sigma used for gaussian blur is non possitive. This will cause unexpected behaviour.");
    }

    textures[0] = texture0;
    textures[1] = texture1;

    for (int i = 0; i < 2; i++) {
        fbo[i] = std::make_shared<GraphicsEngine::Framebuffer>(width, height);
        fbo[i]->attachTexture(textures[i^1], GL_COLOR_ATTACHMENT0);
    }

    using namespace GraphicsEngine::Global;

    graphics.addShader(blurShaderId, {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/standard/gaussianBlur.frag"});

    loadShaderWithParam();
}

GaussianBlur::~GaussianBlur() { }

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

    graphics.getActiveShader()->setSampler("MainTex", 0);
}

void GaussianBlur::blur(int iterations, std::shared_ptr<GraphicsEngine::FullscreenQuad> fullScreenQuad) {
    while (iterations-->0) {
        using namespace GraphicsEngine::Global;

        graphics.bindShader(blurShaderId);

        auto blurShader = graphics.getActiveShader();

        for (int horizontal = 0; horizontal <= 1; horizontal++) {
            fbo[horizontal]->bind();
            blurShader->setInt("horizontal", horizontal);
            textures[horizontal]->bind(GL_TEXTURE0);

            fullScreenQuad->bind();
            fullScreenQuad->draw();
            fullScreenQuad->unbind();

            textures[horizontal]->unbind(GL_TEXTURE0);
        }
    }
}

}
