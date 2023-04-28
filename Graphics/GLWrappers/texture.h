#pragma once

#include "GL/glew.h"
#include <string>

#include "glm/glm.hpp"

namespace GraphicsEngine {
class Texture {
public:
    Texture(int width, int height, GLenum texUnit = GL_TEXTURE0,
            GLint format = GL_RGBA, GLint internalFormat = GL_RGB, GLint dataType = GL_UNSIGNED_BYTE,
            GLenum interpolationMode = GL_LINEAR_MIPMAP_LINEAR, GLenum wrapMode = GL_REPEAT,
            GLenum texTarget = GL_TEXTURE_2D);

    Texture(std::string filePath, GLenum texUnit = GL_TEXTURE0,
            GLint format = GL_RGBA, GLint internalFormat = GL_RGB, GLint dataType = GL_UNSIGNED_BYTE,
            GLenum texTarget = GL_TEXTURE_2D,
            GLenum interpolationMode = GL_LINEAR, GLenum wrapMode = GL_REPEAT);

    ~Texture();

    void bind();
    void bind(GLenum texUnit);
    void unbind();
    void unbind(GLenum texUnit);
    GLuint getHandle();
    GLuint getTexUnitUint();
    GLenum getTexUnitEnum();

private:
    GLuint m_handle;
    GLenum m_texUnit;
    GLenum m_texTarget;
};
}
