#include "drawSystem.h"
#include "Engine/Components/drawSystemData.h"
#include "Engine/Graphics/skybox.h"
#include "Engine/Systems/helpers/postProcessing.h"
#include "Engine/Systems/helpers/shadowMap.h"
#include "Engine/_Core/logger.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/global.h"
#include "../Gameworld/gameworld.h"
#include "../Components/transform.h"
#include "../Components/mesh.h"
#include "../Components/camera.h"
#include "../Components/material.h"
#include "../Components/light.h"
#include "../_Core/asserts.h"
#include "Graphics/light.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/gtx/string_cast.hpp"
#include "imgui.h"
#include <cmath>
#include <iostream>
#include <memory>

namespace Saga::Systems {

namespace {

inline void renderAllShapes(std::shared_ptr<GameWorld> world) {
    using namespace GraphicsEngine::Global;

    // loop through and draw all entities
    for (auto &[entity, material, mesh, transform] : *world->viewGroup<Material, Mesh, Transform>()) {
        SASSERT_MESSAGE(material->material, "Material cannot be null.");
        SASSERT_MESSAGE(mesh->mesh, "Mesh shape cannot be null.");
        SASSERT_MESSAGE(transform->transform, "Transform cannot be null.");

        graphics.drawShape(*mesh, *transform, *material);
    }
}

inline void renderScene(std::shared_ptr<GameWorld> world, Saga::Camera& camera, std::optional<glm::mat4> shadowMapLightSpaceMatrix) {
    using namespace GraphicsEngine::Global;

    Graphics::usePostProcessingFBO(world);
    // graphics setup
    glViewport(0, 0, camera.camera->getWidth(), camera.camera->getHeight());


    // clear color buffer
    if (camera.clearColorBufferOnDraw) {
        graphics.setClearColor(camera.clearColor);
        graphics.clearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else graphics.clearScreen(GL_DEPTH_BUFFER_BIT);


    // set shader
    SASSERT_MESSAGE(camera.shader != "", "Camera cannot have an empty shader");
    graphics.bindShader(camera.shader);
    std::vector<std::shared_ptr<GraphicsEngine::Light>> lights;
    for (auto &[entity, light, transform] : *world->viewGroup<Light, Transform>()) {
        SASSERT_MESSAGE(light->light, "Light reference cannot be null.");
        SASSERT_MESSAGE(transform->transform, "Transform reference cannot be null.");

        light->light->setPos(transform->transform->getPos());
        lights.push_back(light->light);
    }
    graphics.setLights(lights);
    graphics.setGlobalData(camera.globalShaderCoefs);
    graphics.setCameraData(camera);

    if (shadowMapLightSpaceMatrix) {
        // if shadow map is supplied with its matrix
        graphics.getActiveShader()->setSampler("shadowMap", 2);
        graphics.getActiveShader()->setMat4("lightSpaceMatrix", shadowMapLightSpaceMatrix.value());

        auto drawData = world->getComponent<DrawSystemData>(world->getMasterEntity());
        if (drawData && drawData->shadowMap)
            drawData->shadowMap->bind();
        renderAllShapes(world);
        if (drawData && drawData->shadowMap)
            drawData->shadowMap->unbind();
    } else {
        // no shadow map supplied
        renderAllShapes(world);
    }

    {
        // draw skybox
        auto drawData = world->getComponent<DrawSystemData>(world->getMasterEntity());
        if (drawData && drawData->skybox) 
            drawData->skybox->draw(camera); 
    }

    Graphics::performPostProcessing(world, camera);
}

}

void drawSystem_OnSetup(std::shared_ptr<GameWorld> world) {
    Graphics::shadowMapSetup(world);
    for (Saga::Camera& camera : *world->viewAll<Camera>())
        Graphics::postProcessingSetup(world, camera);
    auto drawData = world->getComponent<DrawSystemData>(world->getMasterEntity());

    if (drawData) drawData->skybox = std::make_shared<Saga::Graphics::Skybox>("Resources/Images/skyboxes/universe/", "png");
}

void drawSystem(std::shared_ptr<Saga::GameWorld> world) {
    ImGui::Begin("Draw System");
    Graphics::drawShadowMapGizmos();
    Graphics::drawPostProcessingGizmos(world);
    ImGui::End();

    for (Saga::Camera& camera : *world->viewAll<Camera>()) {
        std::optional<glm::mat4> lightSpaceMatrix = Graphics::renderShadowMap(world, camera);
        renderScene(world, camera, lightSpaceMatrix);
    }
}

void drawSystem_OnResize(std::shared_ptr<GameWorld> world, int width, int height) {
    for (Saga::Camera& camera : *world->viewAll<Camera>()) {
        camera.camera->resize(width, height);
        Graphics::postProcessingSetup(world, camera);
    }
}

void registerDrawSystem(std::shared_ptr<GameWorld> world) {
    world->registerGroup<Material, Mesh, Transform>();
    world->registerGroup<Light, Transform>();
    world->getSystems().addStagedSystem(Saga::System<>(Saga::Systems::drawSystem), Saga::SystemManager::Stage::Draw);
    world->getSystems().addStagedSystem(Saga::System<>(Saga::Systems::drawSystem_OnSetup), Saga::SystemManager::Stage::Awake);
    world->getSystems().addWindowResizeSystem(Saga::Systems::drawSystem_OnResize);
}

} // namespace Saga::System
