#include "framebuffer.h"
#include "Engine/_Core/asserts.h"
#include "Engine/_Core/logger.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/debug.h"
#include <GL/gl.h>

namespace GraphicsEngine {
    Framebuffer::Framebuffer(int width, int height) : width(width), height(height) {
        glGenFramebuffers(1, &handle);
        bind();
        glViewport(0, 0, width, height);
        unbind();
        STRACE("FRAMEBUFFER CREATED with id %d", handle);
    }

    Framebuffer::~Framebuffer() {
        glDeleteFramebuffers(1, &handle);
    }

    void Framebuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, handle);
    }

    void Framebuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        Debug::checkGLError();
    }

    void Framebuffer::verifyStatus() {
        bind();
        GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        SASSERT_MESSAGE(err == GL_FRAMEBUFFER_COMPLETE, "Frame buffer exit with the following status: %d", err);
        unbind();
    }

    GLuint Framebuffer::getHandle() {
        return handle;
    }

    void Framebuffer::disableColorDraw() {
        bind();
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    void Framebuffer::attachTexture(std::shared_ptr<Texture> texture, GLenum attachment) {
        bind();
        Debug::checkGLError();
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->getHandle(), 0);
        if (attachment != GL_DEPTH_ATTACHMENT && attachment != GL_DEPTH_STENCIL_ATTACHMENT) {
            if (std::find(attachments.begin(), attachments.end(), attachment) == attachments.end()) {
                attachments.push_back(attachment);
                glDrawBuffers(attachments.size(), attachments.data());
            }
        }
        unbind();
    }

    std::shared_ptr<Texture> Framebuffer::createAndAttachColorTexture(GLenum attachment, GLenum texUnit) {
        std::shared_ptr<Texture> texture = std::make_shared<Texture>(
            width, height, texUnit, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE
        );
        attachTexture(texture, attachment);
        Debug::checkGLError();
        return texture;
    }

    std::shared_ptr<Texture> Framebuffer::createAndAttachVector3Texture(GLenum attachment, GLenum texUnit) {
        std::shared_ptr<Texture> texture = std::make_shared<Texture>(
            width, height, texUnit, GL_RGBA16F, GL_RGBA, GL_FLOAT
        );
        attachTexture(texture, attachment);
        Debug::checkGLError();
        return texture;
    }

    std::shared_ptr<Texture> Framebuffer::createAndAttachFloatTexture(GLenum attachment, GLenum texUnit) {
        std::shared_ptr<Texture> texture = std::make_shared<Texture>(
            width, height, texUnit, GL_RED, GL_RED, GL_UNSIGNED_BYTE
        );
        attachTexture(texture, attachment);
        Debug::checkGLError();
        return texture;
    }

    std::shared_ptr<Texture> Framebuffer::createAndAttachDepthTexture(GLenum texUnit, GLenum interpolationMode, GLenum wrapMode) {
        std::shared_ptr<Texture> texture = std::make_shared<Texture>(
            width, height, texUnit, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, interpolationMode, wrapMode
        );
        attachTexture(texture, GL_DEPTH_ATTACHMENT);
        Debug::checkGLError();
        return texture;
    }

    std::shared_ptr<Texture> Framebuffer::createAndAttachDepthStencilTexture(GLenum texUnit) {

        std::shared_ptr<Texture> texture = std::make_shared<Texture>(
            width, height, texUnit, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_LINEAR, GL_CLAMP_TO_EDGE
        );
        attachTexture(texture, GL_DEPTH_STENCIL_ATTACHMENT);
        Debug::checkGLError();
        return texture;
    }

    void Framebuffer::createAndAttachDepthStencilRenderBuffer() {
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glNamedFramebufferRenderbuffer(handle, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        Debug::checkGLError();
    }
}