#pragma once

#include "Graphics/GLWrappers/vao.h"
#include "Graphics/GLWrappers/vbo.h"
#include <vector>
#include <string>

namespace GraphicsEngine {
class FullscreenQuad
{
public:
    FullscreenQuad();
    ~FullscreenQuad();

    void bind();
    void unbind();
    void draw();

private:
    std::shared_ptr<GraphicsEngine::VAO> vao;
    std::shared_ptr<GraphicsEngine::VBO> vbo;
};
}
