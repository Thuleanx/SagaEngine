#include "depthOfField.h"
#include "Engine/Graphics/blit.h"
#include "Engine/Graphics/gaussianBlur.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/debug.h"
#include "Graphics/global.h"
#include "Engine/_Core/asserts.h"
#include <memory>

namespace Saga::Graphics {

int DepthOfField::newUID = 0;

DepthOfField::DepthOfField(int width, int height, std::shared_ptr<GraphicsEngine::Texture> resultTex) :
    width(width), height(height), uid(newUID++), splitFBO("doffbo0#" + std::to_string(uid)),
    combineFBO("doffbo1#" + std::to_string(uid))  {

    generateTextures();

    using namespace GraphicsEngine::Global;

    SASSERT(splitFBO != combineFBO);

    graphics.addFramebuffer(splitFBO, width, height);
    {
        auto framebuffer = graphics.getFramebuffer(splitFBO);
        framebuffer->attachTexture(coc, GL_COLOR_ATTACHMENT0);
        framebuffer->attachTexture(cocNear[0], GL_COLOR_ATTACHMENT1);
        framebuffer->attachTexture(foregroundBlurred[0], GL_COLOR_ATTACHMENT2);
        framebuffer->attachTexture(backgroundBlurred[0], GL_COLOR_ATTACHMENT3);
        framebuffer->verifyStatus();
    }

    graphics.addFramebuffer(combineFBO, width, height);
    {
        auto framebuffer = graphics.getFramebuffer(combineFBO);
        framebuffer->attachTexture(resultTex, GL_COLOR_ATTACHMENT0);
        framebuffer->verifyStatus();
    }

    graphics.addShader(depthOfFieldShaderID,
    {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/depthOfField/dofSplit.frag"});

    graphics.addShader(depthOfFieldCombineShaderID,
    {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/depthOfField/dofCombine.frag"});

    graphics.bindShader(depthOfFieldShaderID);
    graphics.getActiveShader()->setSampler("MainTex", 0); // sample from GL_TEXTURE0
    graphics.getActiveShader()->setSampler("DepthTex", 1); // sample from GL_TEXTURE1

    graphics.bindShader(depthOfFieldCombineShaderID);
    graphics.getActiveShader()->setSampler("MainTex", 0);
    graphics.getActiveShader()->setSampler("coc", 1);
    graphics.getActiveShader()->setSampler("cocNear", 2);
    graphics.getActiveShader()->setSampler("foregroundBlurred", 3);
    graphics.getActiveShader()->setSampler("backgroundBlurred", 4);

    foregroundBlur = std::make_shared<GaussianBlur>(width, height, 
        foregroundBlurred[0],
        foregroundBlurred[1]
    );

    backgroundBlur = std::make_shared<GaussianBlur>(width, height, 
        backgroundBlurred[0],
        backgroundBlurred[1]
    );

    cocNearExpand = std::make_shared<MaxFilter>(width, height, cocNear[0], cocNear[1], 10);
    cocNearBlur = std::make_shared<GaussianBlur>(width, height, cocNear[0], cocNear[1]);
}

DepthOfField::~DepthOfField() {
    using namespace GraphicsEngine::Global;
    graphics.removeFramebuffer(splitFBO);
    graphics.removeFramebuffer(combineFBO);
}

void DepthOfField::loadShaderWithParam(Saga::Camera& camera, float focusDistance, float focusRange) {
    using namespace GraphicsEngine::Global;
    auto shader = graphics.getActiveShader();

    shader->setFloat("near", camera.camera->getNear());
    shader->setFloat("far", camera.camera->getFar());
    shader->setFloat("focusDistance", focusDistance);
    shader->setFloat("focusRange", focusRange);
}

void DepthOfField::apply(Saga::Camera& camera, 
        std::shared_ptr<GraphicsEngine::Texture> mainTex, std::shared_ptr<GraphicsEngine::Texture> depthTex,
        float focalDistance, float focalRange, int foregroundBlurIterations, int backgroundBlurIterations,
        int sizeOfNearExpand, int nearCocBlurIterations) {

    using namespace GraphicsEngine::Global;

    glViewport(0, 0, width, height);

    glDisable(GL_BLEND);

    // first process the screen, compute coc, and split into textures
    graphics.bindFramebuffer(splitFBO);
    graphics.bindShader(depthOfFieldShaderID);
    loadShaderWithParam(camera, focalDistance, focalRange);
    depthTex->bind(GL_TEXTURE1);
    blit(splitFBO, depthOfFieldShaderID, mainTex);

    glDisable(GL_BLEND);
    // expand and blur out nearCoc
    cocNearExpand->setSize(sizeOfNearExpand);
    cocNearExpand->apply(1);
    cocNearBlur->apply(nearCocBlurIterations);

    // blur background and foreground
    foregroundBlur->apply(foregroundBlurIterations);
    backgroundBlur->apply(backgroundBlurIterations);

    // combine into depth of field result
    graphics.bindFramebuffer(combineFBO);
    graphics.bindShader(depthOfFieldCombineShaderID);
    mainTex->bind(GL_TEXTURE0);
    coc->bind(GL_TEXTURE1);
    cocNear[0]->bind(GL_TEXTURE2);
    foregroundBlurred[0]->bind(GL_TEXTURE3);
    backgroundBlurred[0]->bind(GL_TEXTURE4);

    blit(combineFBO, depthOfFieldCombineShaderID, mainTex);

    // unbind stuff, not really neccessary
    mainTex->unbind(GL_TEXTURE0);
    coc->unbind(GL_TEXTURE1);
    cocNear[0]->unbind(GL_TEXTURE1);
    foregroundBlurred[0]->unbind(GL_TEXTURE3);
    backgroundBlurred[0]->unbind(GL_TEXTURE4);

    glEnable(GL_BLEND);
}

void DepthOfField::generateTextures() {
    for (int i = 0; i < 2; i++) {
        foregroundBlurred[i] = std::make_shared<GraphicsEngine::Texture>();
        foregroundBlurred[i]->initialize2D(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        backgroundBlurred[i] = std::make_shared<GraphicsEngine::Texture>();
        backgroundBlurred[i]->initialize2D(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
        cocNear[i] = std::make_shared<GraphicsEngine::Texture>();
        cocNear[i]->initialize2D(width, height, GL_R16F, GL_RED, GL_FLOAT);
    }

    coc = std::make_shared<GraphicsEngine::Texture>();
    coc->initialize2D(width, height, GL_R16F, GL_RED, GL_FLOAT);

}

}

