#pragma once

#include "Graphics/GLWrappers/texture.h"
#include <string>
#include <memory>

namespace Saga::Graphics {
    /**
     * @brief Blit a shader through a framebuffer.
     *
     * @param framebuffer the framebuffer name as stored in the graphics engine.
     * @param shader the shader name as stored in the graphics engine.
     * @param mainTex the texture to blit. THis will automatically be loaded into the GL_TEXTURE0 slot of the shader.
     */
    void blit(const std::string &framebuffer, const std::string &shader, std::shared_ptr<GraphicsEngine::Texture> mainTex);
}
