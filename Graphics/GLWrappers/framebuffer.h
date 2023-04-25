#pragma once

#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

namespace GraphicsEngine {
class Texture;

class Framebuffer {
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    void bind();
    void unbind();
    GLuint getHandle();

    void attachTexture(std::shared_ptr<Texture> texture, GLenum attachment = GL_COLOR_ATTACHMENT0);

    std::shared_ptr<Texture> createAndAttachColorTexture(GLenum attachment = GL_COLOR_ATTACHMENT0, GLenum texUnit = GL_TEXTURE0);
    std::shared_ptr<Texture> createAndAttachVector3Texture(GLenum attachment = GL_COLOR_ATTACHMENT0, GLenum texUnit = GL_TEXTURE0);
    std::shared_ptr<Texture> createAndAttachFloatTexture(GLenum attachment = GL_COLOR_ATTACHMENT0, GLenum texUnit = GL_TEXTURE0);

    std::shared_ptr<Texture> createAndAttachDepthTexture(GLenum texUnit = GL_TEXTURE0);
    std::shared_ptr<Texture> createAndAttachDepthStencilTexture(GLenum texUnit = GL_TEXTURE0);

    void createAndAttachDepthStencilRenderBuffer();
private:
    GLuint handle;
    GLuint rbo;
    int width, height;
    std::vector<GLuint> attachments;
};
}
