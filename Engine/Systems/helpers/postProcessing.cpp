#include "postProcessing.h"
#include "Engine/Components/camera.h"
#include "Engine/Components/drawSystemData.h"
#include "Engine/Gameworld/gameworld.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/debug.h"
#include "Graphics/fullscreenquad.h"
#include "Graphics/global.h"
#include "glm/fwd.hpp"
#include "imgui.h"

namespace Saga::Systems::Graphics {

namespace {
inline void blit(DrawSystemData* drawSystemData, const std::string &framebuffer, const std::string &shader, std::shared_ptr<GraphicsEngine::Texture> mainTex);
}

const std::string screenFramebuffer = "screenFramebuffer";
const std::string bloomExtractionFramebuffer = "bloomExtraction";
const std::string horizontalBlurFramebuffer = "horizontalBlur";
const std::string verticalBlurFramebuffer = "verticalBlur";
const std::string bloomExtractionShader = "bloomExtraction";
const std::string blurShader = "gaussianBlur";
const std::string bloomShader = "bloom";

float bloomThreshold = 0.7;
float bloomExposure = 0.3;
float toneMappingGamma = 2.2;

void postProcessingSetup(std::shared_ptr<Saga::GameWorld> world, Camera& camera) {
    using namespace GraphicsEngine::Global;

    DrawSystemData* drawSystemData =
        world->hasComponent<DrawSystemData>(world->getMasterEntity()) ?
        world->getComponent<DrawSystemData>(world->getMasterEntity()) :
        world->emplace<DrawSystemData>(world->getMasterEntity());

    if (!drawSystemData->fullScreenQuad)
        drawSystemData->fullScreenQuad = std::make_shared<GraphicsEngine::FullscreenQuad>();

    SINFO("Attempt to createframebuffers for post processing");

screenFramebuffer: {
        graphics.addFramebuffer(screenFramebuffer, camera.camera->getWidth(), camera.camera->getHeight());
        auto screenFBO = graphics.getFramebuffer(screenFramebuffer);

        drawSystemData->screenFragmentColor = screenFBO->createAndAttachColorTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE0);
        screenFBO->createAndAttachDepthStencilRenderBuffer();

        screenFBO->verifyStatus();
    }

    SINFO("Created screen framebuffer.");

extractionFramebuffer: {
        graphics.addFramebuffer(bloomExtractionFramebuffer, camera.camera->getWidth(), camera.camera->getHeight());
        auto extraction = graphics.getFramebuffer(bloomExtractionFramebuffer);

        drawSystemData->extractedBrightColor = extraction->createAndAttachColorTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE0);

        extraction->createAndAttachDepthStencilRenderBuffer();

        extraction->verifyStatus();
    }

horizontalBlurFramebuffer: {
        graphics.addFramebuffer(horizontalBlurFramebuffer, camera.camera->getWidth(), camera.camera->getHeight());
        auto horizontalBlurFBO = graphics.getFramebuffer(horizontalBlurFramebuffer);

        drawSystemData->extractedBrightColor2 = horizontalBlurFBO->createAndAttachColorTexture(GL_COLOR_ATTACHMENT0);
        horizontalBlurFBO->verifyStatus();
    }

verticalBlurFramebuffer: {
        graphics.addFramebuffer(verticalBlurFramebuffer, camera.camera->getWidth(), camera.camera->getHeight());
        auto verticalBlurFBO = graphics.getFramebuffer(verticalBlurFramebuffer);

        verticalBlurFBO->attachTexture(drawSystemData->extractedBrightColor, GL_COLOR_ATTACHMENT0);
        verticalBlurFBO->verifyStatus();
    }

    SINFO("Created framebuffers for post processing");

    graphics.addShader(bloomExtractionShader,
    {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/bloom/brightExtractor.frag"});

    graphics.addShader(blurShader,
    {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/standard/gaussianBlur.frag"});

    graphics.addShader(bloomShader,
    {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/bloom/blend.frag"});

    SINFO("Bloom shaders setup successful");
    Debug::checkGLError();
}

void usePostProcessingFBO() {
    using namespace GraphicsEngine::Global;
    graphics.getFramebuffer(screenFramebuffer)->bind();
    /* graphics.bindDefaultFramebuffer(); */
}

void performPostProcessing(std::shared_ptr<Saga::GameWorld> world, Camera& camera) {
    using namespace GraphicsEngine::Global;
    // configure viewport size
    DrawSystemData* drawSystemData =
        world->hasComponent<DrawSystemData>(world->getMasterEntity()) ?
        world->getComponent<DrawSystemData>(world->getMasterEntity()) :
        world->emplace<DrawSystemData>(world->getMasterEntity());

    if (!drawSystemData->fullScreenQuad ||
            !drawSystemData->screenFragmentColor ||
            !drawSystemData->extractedBrightColor ||
            !drawSystemData->extractedBrightColor2) {

        SERROR("It seems like post processing stack has not been properly set up");
        return;
    }

    glDisable(GL_DEPTH_TEST);

    graphics.bindShader(bloomExtractionShader);
    graphics.getActiveShader()->setFloat("threshold", bloomThreshold);
    blit(drawSystemData, bloomExtractionFramebuffer, 
        bloomExtractionShader, drawSystemData->screenFragmentColor);
    Debug::checkGLError();

    graphics.bindShader(blurShader);
    graphics.getShader(blurShader)->setInt("horizontal", 1);

    blit(drawSystemData, horizontalBlurFramebuffer, 
        blurShader, drawSystemData->extractedBrightColor);
    Debug::checkGLError();

    graphics.bindShader(blurShader);
    graphics.getShader(blurShader)->setInt("horizontal", 0);

    blit(drawSystemData, verticalBlurFramebuffer, 
        blurShader, drawSystemData->extractedBrightColor2);
    Debug::checkGLError();

    graphics.bindShader(bloomShader);
    graphics.getShader(bloomShader)->setSampler("bloom", 1);
    graphics.getShader(bloomShader)->setFloat("exposure", bloomExposure);
    graphics.getShader(bloomShader)->setFloat("gamma", toneMappingGamma);

    drawSystemData->extractedBrightColor->bind(GL_TEXTURE1);
    blit(drawSystemData, "", bloomShader, drawSystemData->screenFragmentColor);
    drawSystemData->extractedBrightColor->unbind(GL_TEXTURE1);
    Debug::checkGLError();

    glEnable(GL_DEPTH_TEST);
}

void drawPostProcessingGizmos() {
    ImGui::Begin("Post Processing");
        ImGui::SliderFloat("bloom threshold", &bloomThreshold, 0.f, 1.f);
        ImGui::SliderFloat("bloom exposure", &bloomExposure, 0, 20);
        ImGui::SliderFloat("tone mapping gamma", &toneMappingGamma, 1, 3);
    ImGui::End();
}

namespace {
void blit(DrawSystemData* drawSystemData, const std::string &framebuffer, const std::string &shader, std::shared_ptr<GraphicsEngine::Texture> mainTex) {
    using namespace GraphicsEngine::Global;

    if (framebuffer.size()) graphics.bindFramebuffer(framebuffer);
    else graphics.bindDefaultFramebuffer();

    graphics.bindShader(shader);
    graphics.getActiveShader()->setSampler("MainTex", 0);
    mainTex->bind(GL_TEXTURE0);

    drawSystemData->fullScreenQuad->bind();
    drawSystemData->fullScreenQuad->draw();
    drawSystemData->fullScreenQuad->unbind();

    mainTex->unbind(GL_TEXTURE0);
}
}

}
