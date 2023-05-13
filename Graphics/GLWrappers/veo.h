#pragma once

#include <GL/glew.h>
#include <stdint.h>
#include <vector>

namespace GraphicsEngine {
class VEO {
public:
    VEO(std::vector<int> data);
    ~VEO();

    void bind();
    void unbind();
    GLsizei getLength();

private:
    GLuint m_handle;
    GLsizei m_length;
};
} // namespace GraphicsEngine
