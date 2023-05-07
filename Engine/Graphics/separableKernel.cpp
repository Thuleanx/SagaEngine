#include "separableKernel.h"
#include "Engine/Graphics/blit.h"
#include "Engine/_Core/logger.h"
#include "Graphics/GLWrappers/framebuffer.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/global.h"

namespace Saga::Graphics {

int SeparableKernel::newUID = 0;

SeparableKernel::SeparableKernel(int width, int height, 
        std::shared_ptr<GraphicsEngine::Texture> texture0, 
        std::shared_ptr<GraphicsEngine::Texture> texture1) :
    uid(newUID++), width(width), height(height) {

    textures[0] = texture0;
    textures[1] = texture1;

    for (int i = 0; i < 2; i++) {
        using namespace GraphicsEngine::Global;
        fbo[i] = "separableKernelFBO_" + std::to_string(uid) + "_" + std::to_string(i);
        graphics.addFramebuffer(fbo[i], width, height);
        graphics.getFramebuffer(fbo[i])->attachTexture(textures[i^1], GL_COLOR_ATTACHMENT0);
        graphics.getFramebuffer(fbo[i])->verifyStatus();
    }
}

SeparableKernel::~SeparableKernel() { 
    using namespace GraphicsEngine::Global;
    graphics.removeFramebuffer(fbo[0]);
    graphics.removeFramebuffer(fbo[1]);
}

void SeparableKernel::apply(int iterations) {
    while (iterations-->0) {
        using namespace GraphicsEngine::Global;

        std::string shader = getShader();
        graphics.bindShader(shader);
        auto blurShader = graphics.getActiveShader();
        for (int horizontal = 0; horizontal <= 1; horizontal++) {
            blurShader->setInt("horizontal", horizontal);
            blit(fbo[horizontal], shader, textures[horizontal]);
        }
    }
}

}
