#include "postProcessing.h"
#include "Engine/Components/camera.h"
#include "Engine/Components/drawSystemData.h"
#include "Engine/Gameworld/gameworld.h"
#include "Engine/Graphics/blit.h"
#include "Engine/Graphics/gaussianBlur.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/debug.h"
#include "Graphics/global.h"
#include "glm/fwd.hpp"
#include "imgui.h"

namespace Saga::Systems::Graphics {

namespace {
void loadPostProcessingValues(DrawSystemData* drawSystemData);
}

void postProcessingSetup(std::shared_ptr<Saga::GameWorld> world, Camera& camera) {
    using namespace GraphicsEngine::Global;

    int width = camera.camera->getWidth(), height = camera.camera->getHeight();

    DrawSystemData* drawSystemData =
        world->hasComponent<DrawSystemData>(world->getMasterEntity()) ?
        world->getComponent<DrawSystemData>(world->getMasterEntity()) :
        world->emplace<DrawSystemData>(world->getMasterEntity());

    SINFO("Attempt to createframebuffers for post processing");

screenFramebuffer: {
        graphics.addFramebuffer(drawSystemData->postProcessingSettings.screenFramebuffer, 
                camera.camera->getWidth(), camera.camera->getHeight());
        auto screenFBO = graphics.getFramebuffer(drawSystemData->postProcessingSettings.screenFramebuffer);

        drawSystemData->screenFragmentColor = screenFBO->createAndAttachColorTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE0,
            GL_RGBA16F, GL_RGBA, GL_FLOAT);
        screenFBO->createAndAttachDepthStencilRenderBuffer();

        screenFBO->verifyStatus();
    }

    SINFO("Created screen framebuffer.");

    drawSystemData->extractedBrightColor = std::make_shared<GraphicsEngine::Texture>();
    drawSystemData->extractedBrightColor->initialize2D(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    drawSystemData->extractedBrightColor->setInterpolation(GL_LINEAR);
    drawSystemData->extractedBrightColor->setWrapping(GL_CLAMP_TO_EDGE);

    drawSystemData->extractedBrightColor2 = std::make_shared<GraphicsEngine::Texture>();
    drawSystemData->extractedBrightColor2->initialize2D(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    drawSystemData->extractedBrightColor2->setInterpolation(GL_LINEAR);
    drawSystemData->extractedBrightColor2->setWrapping(GL_CLAMP_TO_EDGE);

extractionFramebuffer: {
        graphics.addFramebuffer(drawSystemData->postProcessingSettings.bloomExtractionFramebuffer, camera.camera->getWidth(), camera.camera->getHeight());
        auto extraction = graphics.getFramebuffer(drawSystemData->postProcessingSettings.bloomExtractionFramebuffer);
        extraction->attachTexture(drawSystemData->extractedBrightColor, GL_COLOR_ATTACHMENT0);
        extraction->verifyStatus();
    }

    drawSystemData->bloomBlur = std::make_shared<Saga::Graphics::GaussianBlur>(width, height, drawSystemData->extractedBrightColor, drawSystemData->extractedBrightColor2);

    SINFO("Created framebuffers for post processing");

    graphics.addShader(drawSystemData->postProcessingSettings.bloomExtractionShader,
    {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/bloom/brightExtractor.frag"});

    graphics.addShader(drawSystemData->postProcessingSettings.postProcessingColors,
    {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/bloom/blend.frag"});

    SINFO("Bloom shaders setup successful");
    Debug::checkGLError();
}

void usePostProcessingFBO(std::shared_ptr<GameWorld> world) {
    using namespace GraphicsEngine::Global;

    DrawSystemData* drawSystemData =
        world->hasComponent<DrawSystemData>(world->getMasterEntity()) ?
        world->getComponent<DrawSystemData>(world->getMasterEntity()) :
        world->emplace<DrawSystemData>(world->getMasterEntity());

    graphics.getFramebuffer(drawSystemData->postProcessingSettings.screenFramebuffer)->bind();
    /* graphics.bindDefaultFramebuffer(); */
}

void performPostProcessing(std::shared_ptr<Saga::GameWorld> world, Camera& camera) {
    using namespace GraphicsEngine::Global;
    // configure viewport size
    DrawSystemData* drawSystemData =
        world->hasComponent<DrawSystemData>(world->getMasterEntity()) ?
        world->getComponent<DrawSystemData>(world->getMasterEntity()) :
        world->emplace<DrawSystemData>(world->getMasterEntity());

    if (!drawSystemData->screenFragmentColor ||
        !drawSystemData->extractedBrightColor ||
        !drawSystemData->extractedBrightColor2) {

        SERROR("It seems like post processing stack has not been properly set up");
        return;
    }

    glDisable(GL_DEPTH_TEST);

    graphics.bindShader(drawSystemData->postProcessingSettings.bloomExtractionShader);
    graphics.getActiveShader()->setFloat("threshold", drawSystemData->postProcessingSettings.bloomThreshold);
    Saga::Graphics::blit(drawSystemData->postProcessingSettings.bloomExtractionFramebuffer, 
        drawSystemData->postProcessingSettings.bloomExtractionShader, drawSystemData->screenFragmentColor);
    Debug::checkGLError();

    drawSystemData->bloomBlur->blur(drawSystemData->postProcessingSettings.bloomBlurIterations);
    Debug::checkGLError();

    graphics.bindShader(drawSystemData->postProcessingSettings.postProcessingColors);
    graphics.getActiveShader()->setSampler("bloom", 1);
    loadPostProcessingValues(drawSystemData);
    drawSystemData->extractedBrightColor->bind(GL_TEXTURE1);
    Saga::Graphics::blit("", drawSystemData->postProcessingSettings.postProcessingColors, drawSystemData->screenFragmentColor);
    drawSystemData->extractedBrightColor->unbind(GL_TEXTURE1);
    Debug::checkGLError();

    glEnable(GL_DEPTH_TEST);
}


void drawPostProcessingGizmos(std::shared_ptr<Saga::GameWorld> world) {
    DrawSystemData* drawSystemData =
        world->hasComponent<DrawSystemData>(world->getMasterEntity()) ?
        world->getComponent<DrawSystemData>(world->getMasterEntity()) :
        world->emplace<DrawSystemData>(world->getMasterEntity());

    ImGui::Begin("Post Processing");
    if (ImGui::CollapsingHeader("Bloom")) {
        ImGui::SliderInt("bloom iterations", &drawSystemData->postProcessingSettings.bloomBlurIterations, 0, 20);
        ImGui::SliderFloat("bloom radius", &drawSystemData->postProcessingSettings.bloomRadius, 1.f, 10.f);
        ImGui::SliderFloat("bloom threshold", &drawSystemData->postProcessingSettings.bloomThreshold, 0.f, 1.f);
    }
    if (ImGui::CollapsingHeader("Color correction")) {
        ImGui::SliderFloat("exposure", &drawSystemData->postProcessingSettings.exposure, 0, 20);
        ImGui::SliderFloat("temperature", &drawSystemData->postProcessingSettings.temperature, -1.66, 1.66);
        ImGui::SliderFloat("tint", &drawSystemData->postProcessingSettings.tint, -1.66, 1.66);
        ImGui::SliderFloat("contrast", &drawSystemData->postProcessingSettings.contrast, 0, 2);
        ImGui::SliderFloat("brightness", &drawSystemData->postProcessingSettings.brightness, -1, 1);
        ImGui::SliderFloat("saturation", &drawSystemData->postProcessingSettings.saturation, 0, 2);
        ImGui::SliderFloat("gamma", &drawSystemData->postProcessingSettings.gamma, 1, 3);
        ImGui::EndMenu();
    }
    ImGui::End();
}

namespace {
void loadPostProcessingValues(DrawSystemData* drawSystemData) {
    using namespace GraphicsEngine::Global;
    graphics.getActiveShader()->setFloat("exposure", drawSystemData->postProcessingSettings.exposure);
    graphics.getActiveShader()->setFloat("temperature", drawSystemData->postProcessingSettings.temperature);
    graphics.getActiveShader()->setFloat("tint", drawSystemData->postProcessingSettings.tint);
    graphics.getActiveShader()->setFloat("contrast", drawSystemData->postProcessingSettings.contrast);
    graphics.getActiveShader()->setFloat("brightness", drawSystemData->postProcessingSettings.brightness);
    graphics.getActiveShader()->setFloat("saturation", drawSystemData->postProcessingSettings.saturation);
    graphics.getActiveShader()->setFloat("gamma", drawSystemData->postProcessingSettings.gamma);
}
}

}
