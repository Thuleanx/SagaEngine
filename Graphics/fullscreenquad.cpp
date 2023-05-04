

#include "fullscreenquad.h"
#include "Engine/_Core/logger.h"
#include "Graphics/GLWrappers/vao.h"
#include <vector>

namespace GraphicsEngine {
FullscreenQuad::FullscreenQuad(){
    std::vector<float> data = {
        -1, -1, 0,
        1, -1, 0,
        1, 1, 0,
        1, 1, 0,
        -1, 1, 0,
        -1, -1, 0
    };
    vbo = std::make_shared<VBO>(data);
    vao = std::make_shared<VAO>(vbo, VAOAttrib::POS);
}

FullscreenQuad::~FullscreenQuad(){ }

FullscreenQuad* FullscreenQuad::bind(){
    vao->bind();
    return this;
}

FullscreenQuad* FullscreenQuad::unbind(){
    vao->unbind();
    return this;
}

FullscreenQuad* FullscreenQuad::draw() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    return this;
}
}

