#include "drawSystem.h"
#include "Engine/Components/drawSystemData.h"
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
#include "glm/gtx/string_cast.hpp"
#include <iostream>

namespace Saga::Systems {

namespace {
void shadowMapSetup(std::shared_ptr<GameWorld> world) {
    using namespace GraphicsEngine::Global;

    SINFO("Attempt to create framebuffer");
    graphics.addFramebuffer("shadowMap", 512, 512);

    auto drawSystemData = world->emplace<DrawSystemData>(world->getMasterEntity());

    SINFO("Created frame buffer for shadow mapping");
    auto shadowMapFBO = graphics.getFramebuffer("shadowMap");
    drawSystemData->shadowMap = shadowMapFBO->createAndAttachDepthTexture(GL_TEXTURE2, GL_NEAREST, GL_REPEAT);
    SINFO("Created depth texture for shadow mapping");
    shadowMapFBO->disableColorDraw();
    shadowMapFBO->verifyStatus();
    SINFO("Shadow map framebuffer setup successful");


    graphics.addShader("shadowMapShader", { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER}, 
            {"Resources/Shaders/shadow/shadowMap.vert", "Resources/Shaders/shadow/shadowMap.frag"});

    SINFO("Shadow map shader setup successful");

    graphics.addShader("shadowTestShader", {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
            {"Resources/Shaders/shadow/shadowMapTest.vert", "Resources/Shaders/shadow/shadowMapTest.frag"});

    SINFO("Shadow test shader setup successful");
}

void renderScene(std::shared_ptr<GameWorld> world) {
    using namespace GraphicsEngine::Global;

    // loop through and draw all entities
    for (auto &[entity, material, mesh, transform] : *world->viewGroup<Material, Mesh, Transform>()) {
        SASSERT_MESSAGE(material->material, "Material cannot be null.");
        SASSERT_MESSAGE(mesh->mesh, "Mesh shape cannot be null.");
        SASSERT_MESSAGE(transform->transform, "Transform cannot be null.");

        graphics.drawShape(*mesh, *transform, *material);
    }
}
}

void drawSystem_OnSetup(std::shared_ptr<GameWorld> world) {
    shadowMapSetup(world);
}

void drawSystem(std::shared_ptr<Saga::GameWorld> world) {
    for (Saga::Camera& camera : *world->viewAll<Camera>()) {
        // shadow pass for shadow mappping
shadowPass: {
            using namespace GraphicsEngine::Global;
            std::optional<Light*> mainLight = world->viewAll<Light>()->any();
            if (mainLight) {
                SASSERT_MESSAGE(mainLight.value()->light->getType() == GraphicsEngine::LightType::DIRECTIONAL, "Currently, we only support shadowmapping for directional light. Ensure that the first light added to the scene is directional.");

                glViewport(0, 0, 512, 512);
                graphics.getFramebuffer("shadowMap")->bind();
                graphics.clearScreen(GL_DEPTH_BUFFER_BIT);

                float near_plane = 1.0f, far_plane = 7.5f;
                glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

                /* glm::vec3 cameraFulstrumCenter = camera.camera->getPos() + camera.camera->getLook() * 2.0f; */
                glm::vec3 cameraFulstrumCenter = glm::vec3(0,0,0);

                glm::vec3 lightViewPos = cameraFulstrumCenter - mainLight.value()->light->getDir() * 6.0f;

                SDEBUG("Light at: %s, %s", 
                    glm::to_string(cameraFulstrumCenter).c_str(), 
                    glm::to_string(lightViewPos).c_str());

                glm::mat4 lightView = glm::lookAt(lightViewPos, cameraFulstrumCenter, glm::vec3(0, 1, 0));

                glm::mat4 lightSpaceMatrix = lightProjection * lightView;

                auto shader = graphics.getShader("shadowMapShader");

                graphics.bindShader("shadowMapShader");
                graphics.getShader("shadowMapShader")->setMat4("lightSpaceMatrix", lightSpaceMatrix);

                renderScene(world);
            }
        }

renderingPass: {
            using namespace GraphicsEngine::Global;
            // graphics setup
            glViewport(0, 0, camera.camera->getWidth(), camera.camera->getHeight());

            graphics.bindDefaultFramebuffer();

            // clear color buffer
            if (camera.clearColorBufferOnDraw) {
                graphics.setClearColor(camera.clearColor);
                graphics.clearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            } else graphics.clearScreen(GL_DEPTH_BUFFER_BIT);

            // set shader
            SASSERT_MESSAGE(camera.shader != "", "Camera cannot have an empty shader");
            graphics.bindShader("shadowTestShader");
            /* graphics.bindShader(camera.shader); */

            graphics.getActiveShader()->setSampler("shadowMap", 2);
            
            auto drawData = world->getComponent<DrawSystemData>(world->getMasterEntity());
            if (drawData && drawData->shadowMap) {
                drawData->shadowMap->bind();
            }

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

            renderScene(world);

            if (drawData && drawData->shadowMap) {
                drawData->shadowMap->unbind();
            }
        }
    }
}

void drawSystem_OnResize(std::shared_ptr<GameWorld> world, int width, int height) {
    for (Saga::Camera& camera : *world->viewAll<Camera>())
        camera.camera->resize(width, height);
}

void registerDrawSystem(std::shared_ptr<GameWorld> world) {
    world->registerGroup<Material, Mesh, Transform>();
    world->registerGroup<Light, Transform>();
    world->getSystems().addStagedSystem(Saga::System<>(Saga::Systems::drawSystem), Saga::SystemManager::Stage::Draw);
    world->getSystems().addStagedSystem(Saga::System<>(Saga::Systems::drawSystem_OnSetup), Saga::SystemManager::Stage::Awake);
    world->getSystems().addWindowResizeSystem(Saga::Systems::drawSystem_OnResize);
}

} // namespace Saga::System
