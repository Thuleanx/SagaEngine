#include "postProcessing.h"
#include "Engine/Components/camera.h"
#include "Engine/Components/drawSystemData.h"
#include "Engine/Gameworld/gameworld.h"
#include "Engine/Graphics/blit.h"
#include "Engine/Graphics/depthOfField.h"
#include "Engine/Graphics/fog.h"
#include "Engine/Graphics/gaussianBlur.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/debug.h"
#include "Graphics/global.h"
#include "glm/fwd.hpp"
#include "imgui.h"
#include <memory>

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

        drawSystemData->screenFragmentColorAfterFog = screenFBO->createAndAttachColorTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE0,
            GL_RGBA16F, GL_RGBA, GL_FLOAT);
        drawSystemData->screenFragmentColor = screenFBO->createAndAttachColorTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE0,
            GL_RGBA16F, GL_RGBA, GL_FLOAT);
        drawSystemData->depthStencil = screenFBO->createAndAttachDepthStencilTexture();

        screenFBO->verifyStatus();
    }

    SINFO("Created screen framebuffer.");

    drawSystemData->bloomColor0 = std::make_shared<GraphicsEngine::Texture>();
    drawSystemData->bloomColor0->initialize2D(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    drawSystemData->bloomColor0->setInterpolation(GL_LINEAR);
    drawSystemData->bloomColor0->setWrapping(GL_CLAMP_TO_EDGE);

    drawSystemData->bloomColor1 = std::make_shared<GraphicsEngine::Texture>();
    drawSystemData->bloomColor1->initialize2D(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    drawSystemData->bloomColor1->setInterpolation(GL_LINEAR);
    drawSystemData->bloomColor1->setWrapping(GL_CLAMP_TO_EDGE);

    drawSystemData->fog = std::make_shared<Saga::Graphics::Fog>(width, height, drawSystemData->screenFragmentColorAfterFog);
    drawSystemData->dof = std::make_shared<Saga::Graphics::DepthOfField>(width, height, drawSystemData->bloomColor0);

extractionFramebuffer: {
        graphics.addFramebuffer(drawSystemData->postProcessingSettings.bloomExtractionFramebuffer, camera.camera->getWidth(), camera.camera->getHeight());
        auto extraction = graphics.getFramebuffer(drawSystemData->postProcessingSettings.bloomExtractionFramebuffer);
        extraction->attachTexture(drawSystemData->bloomColor0, GL_COLOR_ATTACHMENT0);
        extraction->verifyStatus();
    }

    drawSystemData->bloomBlur = std::make_shared<Saga::Graphics::GaussianBlur>(width, height, drawSystemData->bloomColor0, drawSystemData->bloomColor1);

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

    if (drawSystemData->postProcessingSettings.enabled)
        graphics.getFramebuffer(drawSystemData->postProcessingSettings.screenFramebuffer)->bind();
    else
        graphics.bindDefaultFramebuffer();
}

void performPostProcessing(std::shared_ptr<Saga::GameWorld> world, Camera& camera) {
    using namespace GraphicsEngine::Global;
    // configure viewport size
    DrawSystemData* drawSystemData =
        world->hasComponent<DrawSystemData>(world->getMasterEntity()) ?
        world->getComponent<DrawSystemData>(world->getMasterEntity()) :
        world->emplace<DrawSystemData>(world->getMasterEntity());

    if (!drawSystemData->postProcessingSettings.enabled) return;

    if (!drawSystemData->screenFragmentColor ||
        !drawSystemData->bloomColor0 ||
        !drawSystemData->bloomColor1) {

        SERROR("It seems like post processing stack has not been properly set up");
        return;
    }

    glDisable(GL_DEPTH_TEST);

    // dof -> bloomColor0
    drawSystemData->dof->apply(camera, 
        drawSystemData->screenFragmentColor,
        drawSystemData->depthStencil,
        drawSystemData->postProcessingSettings.focusDistance, 
        drawSystemData->postProcessingSettings.focusRange, 5, 5);

    // fog -> screenFragmentColorAfterFog
    drawSystemData->fog->applyFog(camera, 
        drawSystemData->bloomColor0,
        drawSystemData->depthStencil,
        drawSystemData->postProcessingSettings.fogColor,
        drawSystemData->postProcessingSettings.fogDensity);
    

    // bloom: extract hdr colors -> tempScreenFragmentColor
    graphics.bindShader(drawSystemData->postProcessingSettings.bloomExtractionShader);
    graphics.getActiveShader()->setFloat("threshold", drawSystemData->postProcessingSettings.bloomThreshold);
    Saga::Graphics::blit(drawSystemData->postProcessingSettings.bloomExtractionFramebuffer, 
        drawSystemData->postProcessingSettings.bloomExtractionShader, drawSystemData->screenFragmentColorAfterFog);
    Debug::checkGLError();

    // bloom: blur -> tempScreenFragmentColor
    drawSystemData->bloomBlur->blur(drawSystemData->postProcessingSettings.bloomBlurIterations);
    Debug::checkGLError();

    // post processing steps + combine bloom with original colors
    graphics.bindShader(drawSystemData->postProcessingSettings.postProcessingColors);
    graphics.getActiveShader()->setSampler("bloom", 1);
    loadPostProcessingValues(drawSystemData);
    drawSystemData->bloomColor0->bind(GL_TEXTURE1);
    Saga::Graphics::blit("", drawSystemData->postProcessingSettings.postProcessingColors, drawSystemData->screenFragmentColorAfterFog);
    drawSystemData->bloomColor0->unbind(GL_TEXTURE1);
    Debug::checkGLError();

    glEnable(GL_DEPTH_TEST);
}


void drawPostProcessingGizmos(std::shared_ptr<Saga::GameWorld> world) {
    DrawSystemData* drawSystemData =
        world->hasComponent<DrawSystemData>(world->getMasterEntity()) ?
        world->getComponent<DrawSystemData>(world->getMasterEntity()) :
        world->emplace<DrawSystemData>(world->getMasterEntity());

    ImGui::Checkbox("Enable post processing", &drawSystemData->postProcessingSettings.enabled);

    if (ImGui::CollapsingHeader("Depth of Field")) {
        ImGui::SliderFloat("focus distance", &drawSystemData->postProcessingSettings.focusDistance, 0, 50);
        ImGui::SliderFloat("focus range", &drawSystemData->postProcessingSettings.focusRange, 0, 10);
    }

    if (ImGui::CollapsingHeader("Fog")) {
        ImGui::SliderFloat("fog density", &drawSystemData->postProcessingSettings.fogDensity, 0, 1);
        ImGui::ColorPicker3("fog color", glm::value_ptr(drawSystemData->postProcessingSettings.fogColor));
    }

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
