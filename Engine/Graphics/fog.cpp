#include "fog.h"
#include "Engine/Graphics/blit.h"
#include "Graphics/global.h"

namespace Saga::Graphics {

int Fog::newUID = 0;

Fog::Fog(int width, int height, std::shared_ptr<GraphicsEngine::Texture> resultTex) :
    width(width), height(height), uid(newUID++), fbo("fogfbo#" + std::to_string(uid)) {

    using namespace GraphicsEngine::Global;
    graphics.addFramebuffer(fbo, width, height);
    auto framebuffer = graphics.getFramebuffer(fbo);
    framebuffer->attachTexture(resultTex, GL_COLOR_ATTACHMENT0);
    framebuffer->verifyStatus();

    graphics.addShader(fogShaderId,
    {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/fog/fog.frag"});

    graphics.bindShader(fogShaderId);
    graphics.getActiveShader()->setInt("MainTex", 0); // sample from GL_TEXTURE0
    graphics.getActiveShader()->setInt("DepthTex", 1); // sample from GL_TEXTURE1
}

Fog::~Fog() {
    using namespace GraphicsEngine::Global;
    graphics.removeFramebuffer(fbo);
}

void Fog::applyFog(Saga::Camera& camera, 
    std::shared_ptr<GraphicsEngine::Texture> mainTex, std::shared_ptr<GraphicsEngine::Texture> depthTex,
    glm::vec3 fogColor, float fogDensity) {

    using namespace GraphicsEngine::Global;

    graphics.bindFramebuffer(fbo);
    graphics.bindShader(fogShaderId);
    loadShaderWithParam(camera, fogColor, fogDensity);
    depthTex->bind(GL_TEXTURE1);
    blit(fbo, fogShaderId, mainTex);
}

void Fog::loadShaderWithParam(Saga::Camera& camera, glm::vec3 fogColor, float fogDensity) {
    using namespace GraphicsEngine::Global;
    auto shader = graphics.getActiveShader();
    shader->setFloat("near", camera.camera->getNear());
    shader->setFloat("far", camera.camera->getFar());
    shader->setVec3("fogColor", fogColor);
    shader->setFloat("fogDensity", fogDensity);

}

}
