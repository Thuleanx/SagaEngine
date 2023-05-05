#pragma once

#include "Graphics/GLWrappers/vao.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace GraphicsEngine {

class CubeMap
{
public:
    CubeMap(std::vector<std::string> filenames, std::vector<GLenum> faces);
    ~CubeMap();

    void bind();
    void unbind();

private:
    GLuint m_handle;
};
}
