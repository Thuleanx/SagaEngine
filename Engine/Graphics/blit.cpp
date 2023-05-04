#include "blit.h"
#include "Engine/_Core/logger.h"
#include "Graphics/global.h"

namespace Saga::Graphics {
    void blit(const std::string &framebuffer, const std::string &shader, std::shared_ptr<GraphicsEngine::Texture> mainTex) {
        using namespace GraphicsEngine::Global;

        if (framebuffer.size()) graphics.bindFramebuffer(framebuffer);
        else graphics.bindDefaultFramebuffer();


        graphics.bindShader(shader);
        graphics.getActiveShader()->setSampler("MainTex", 0);
        mainTex->bind(GL_TEXTURE0);

        graphics.getFullScreenQuad()->bind()->draw()->unbind();
    }
}
