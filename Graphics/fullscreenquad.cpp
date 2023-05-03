

#include "fullscreenquad.h"
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

FullscreenQuad::~FullscreenQuad(){}

void FullscreenQuad::bind(){
    vao->bind();
}

void FullscreenQuad::unbind(){
    vao->unbind();
}

void FullscreenQuad::draw() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
}

