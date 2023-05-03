#include "shadowMap.h"
#include "Engine/Components/drawSystemData.h"
#include "Engine/Components/light.h"
#include "Engine/Components/material.h"
#include "Engine/Components/mesh.h"
#include "Engine/Components/transform.h"
#include "Engine/Gameworld/gameworld.h"
#include "Engine/Utils/random.h"
#include "Engine/_Core/logger.h"
#include "Graphics/GLWrappers/shader.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/global.h"
#include "Graphics/graphics.h"
#include "imgui.h"
#include <cmath>
#include <string>

namespace Saga::Systems::Graphics {
    const int PCF_KERNEL_SIZE = 64;
    const int SHADOW_WIDTH = 1024;
    const int SHADOW_HEIGHT = 1024;

    float pcf_kernel_radius = 5;
    float near_plane = 1.0f, far_plane = 50.0f;
    float mapped_area = 10.0f;
    float distance_to_camera = 40.0f;

    namespace {
        /**
         * @brief Sets up the kernel of random displacements for 
         *  use in calculating percentage closer filtering.
         *  This generates an assortment of points defined by
         *  pcf_kernel_radius and PCF_KERNEL_SIZE and loads it into 
         *  the array "kernel" on the shader.
         *
         * @param shader the name of the shader to load the kernel into.
         */
        void shadowMapKernelSetup(std::string shader);
    }

    void shadowMapSetup(std::shared_ptr<GameWorld> world) {
        using namespace GraphicsEngine::Global;
        if (graphics.getFramebuffer("shadowMap")) {
            SERROR("Shadow map already set up. Ignore function.");
            return;
        }

        SINFO("Attempt to create framebuffer");
        graphics.addFramebuffer("shadowMap", SHADOW_WIDTH, SHADOW_HEIGHT);

        // setup draw data
        DrawSystemData* drawSystemData = 
            world->hasComponent<DrawSystemData>(world->getMasterEntity()) ? 
                world->getComponent<DrawSystemData>(world->getMasterEntity()) :
                world->emplace<DrawSystemData>(world->getMasterEntity());

        SINFO("Created frame buffer for shadow mapping");
        auto shadowMapFBO = graphics.getFramebuffer("shadowMap");

        // setup shadow map texture
        drawSystemData->shadowMap = shadowMapFBO->createAndAttachDepthTexture(GL_TEXTURE2, GL_NEAREST, GL_CLAMP_TO_BORDER);
        drawSystemData->shadowMap->setBorderColor(glm::vec4(1.0f,1.0f,1.0f,1.0f));
        SINFO("Created depth texture for shadow mapping");
        shadowMapFBO->disableColorDraw();
        shadowMapFBO->verifyStatus();
        SINFO("Shadow map framebuffer setup successful");


        graphics.addShader("shadowMapShader", { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
        {"Resources/Shaders/shadow/shadowMap.vert", "Resources/Shaders/shadow/shadowMap.frag"});

        SINFO("Shadow map shader setup successful.");

        graphics.addShader("shadowTestShader", {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
        {"Resources/Shaders/shadow/shadowMapTest.vert", "Resources/Shaders/shadow/shadowMapTest.frag"});

        SINFO("Shadow test shader setup successful.");

        shadowMapKernelSetup("phong");

        SINFO("Loaded default shader with pcf kernel.");
    }

    std::optional<glm::mat4> renderShadowMap(std::shared_ptr<Saga::GameWorld> world, Saga::Camera &camera) {
        glm::mat4 lightSpaceMatrix;
        glCullFace(GL_FRONT);

        using namespace GraphicsEngine::Global;
        std::optional<Light*> mainLight = world->viewAll<Light>()->any();
        if (mainLight) {
            SASSERT_MESSAGE(mainLight.value()->light->getType() == GraphicsEngine::LightType::DIRECTIONAL, "Currently, we only support shadowmapping for directional light. Ensure that the first light added to the scene is directional.");
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            graphics.getFramebuffer("shadowMap")->bind();
            graphics.clearScreen(GL_DEPTH_BUFFER_BIT);

            glm::mat4 lightProjection = glm::ortho(
                    -mapped_area, mapped_area,
                    -mapped_area, mapped_area, 
                    near_plane, far_plane);

            glm::vec3 focusPosition = glm::vec3(0,0,0);
            glm::vec3 lightDir = glm::normalize(mainLight.value()->light->getDir());
            glm::vec3 lightPos = focusPosition - lightDir * distance_to_camera;

            glm::vec3 up = glm::vec3(0,1,0);
            // might cause discontinueties
            if (std::abs(glm::dot(up, lightDir) - 1))
                up = glm::normalize(glm::vec3(.1,0.9,.1));

            glm::mat4 lightView = glm::lookAt(lightPos, focusPosition, up);
            lightSpaceMatrix = lightProjection * lightView;


            graphics.bindShader("shadowMapShader");
            graphics.getActiveShader()->setMat4("lightSpaceMatrix", lightSpaceMatrix);

            for (auto &[entity, material, mesh, transform] : *world->viewGroup<Material, Mesh, Transform>()) {
                // we need only load the model matrix
                graphics.getActiveShader()->setModelTransform(transform->transform->getModelMatrix());
                mesh->mesh->draw();
            }
        }
        glCullFace(GL_BACK);

        return lightSpaceMatrix;
    }

    void drawShadowMapGizmos() {
        ImGui::Begin("Shadow Map");
            ImGui::SliderFloat("near_plane", &near_plane, 0.01f, 20.0f);
            ImGui::SliderFloat("far_plane", &far_plane, 5.0f, 100.0f);
            ImGui::SliderFloat("mapped_area", &mapped_area, 1.0f, 20.0f);
            ImGui::SliderFloat("distance_to_camera", &distance_to_camera, 1.0f, 50.0f);
        ImGui::End();
    }

    namespace {
        void shadowMapKernelSetup(std::string shader) {
            using namespace GraphicsEngine::Global;
            graphics.bindShader(shader);

            std::vector<glm::vec2> pcf_kernel(PCF_KERNEL_SIZE);
            for (int i = 0; i < PCF_KERNEL_SIZE; i++)
                pcf_kernel[i] = Saga::Random::getUniformUnitDisk() * pcf_kernel_radius;

            auto shaderPtr = graphics.getActiveShader();

            for (int i = 0; i < PCF_KERNEL_SIZE; i++) {
                std::string prop = "kernel[" + std::to_string(i) + "]";
                shaderPtr->setVec2(prop, pcf_kernel[i]);
            }
        }
    }
}
