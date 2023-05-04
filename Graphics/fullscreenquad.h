#pragma once

#include "Graphics/GLWrappers/vao.h"
#include "Graphics/GLWrappers/vbo.h"
#include <memory>
#include <vector>
#include <string>

namespace GraphicsEngine {
class FullscreenQuad
{
public:
    FullscreenQuad();
    ~FullscreenQuad();

    FullscreenQuad* bind();
    FullscreenQuad* unbind();
    FullscreenQuad* draw();

private:
    std::shared_ptr<GraphicsEngine::VAO> vao;
    std::shared_ptr<GraphicsEngine::VBO> vbo;
};
}
