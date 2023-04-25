#include "texture.h"
#include "Engine/_Core/logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

#include "../debug.h"

using namespace GraphicsEngine;

Texture::Texture(int width, int height, GLenum texUnit,
                 GLint format, GLint internalFormat, GLint dataType,
                 GLenum texTarget,
                 GLenum interpolationMode, GLenum wrapMode):
    m_texTarget(texTarget),
    m_texUnit(texUnit)
{
    glGenTextures(1, &m_handle);
    bind();
    glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(m_texTarget, GL_TEXTURE_MIN_FILTER, interpolationMode);
    glTexParameteri(m_texTarget, GL_TEXTURE_MAG_FILTER, interpolationMode);
    glTexImage2D(m_texTarget, 0, internalFormat, width, height, 0, format, dataType, NULL);
    unbind();
}

Texture::Texture(std::string filepath, GLenum texUnit,
                 GLint format, GLint internalFormat, GLint dataType,
                 GLenum texTarget,
                 GLenum interpolationMode, GLenum wrapMode):
    m_texTarget(texTarget),
    m_texUnit(texUnit)
{
    glGenTextures(1, &m_handle);
    bind();
    glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(m_texTarget, GL_TEXTURE_MIN_FILTER, interpolationMode);
    glTexParameteri(m_texTarget, GL_TEXTURE_MAG_FILTER, interpolationMode);
    stbi_set_flip_vertically_on_load(1);
    int width, height, numChannels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &numChannels, 4);
    if (stbi_failure_reason()) {
        SERROR(stbi_failure_reason());
    }
    glTexImage2D(m_texTarget, 0, internalFormat, width, height, 0, format, dataType, data);
    stbi_image_free(data);
    unbind();
}

Texture::~Texture() {
    glDeleteTextures(1, &m_handle);
}

void Texture::bind() {
    glActiveTexture(m_texUnit);
    glBindTexture(m_texTarget, m_handle);
}

void Texture::bind(GLenum texUnit) {
    glActiveTexture(texUnit);
    glBindTexture(m_texTarget, m_handle);
}

void Texture::unbind() {
    glActiveTexture(m_texUnit);
    glBindTexture(m_texTarget, 0);
}

void Texture::unbind(GLenum texUnit) {
    glActiveTexture(texUnit);
    glBindTexture(m_texTarget, 0);
}

GLuint Texture::getHandle() {
    return m_handle;
}

GLuint Texture::getTexUnitUint() {
    return GLuint(m_texUnit) - GLuint(GL_TEXTURE0);
}

GLenum Texture::getTexUnitEnum() {
    return m_texUnit;
}
