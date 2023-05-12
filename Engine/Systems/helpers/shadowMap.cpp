#include "shadowMap.h"
#include "Engine/Components/drawSystemData.h"
#include "Engine/Components/light.h"
#include "Engine/Components/material.h"
#include "Engine/Components/mesh.h"
#include "Engine/Components/transform.h"
#include "Engine/Datastructures/Accelerant/boundingBox.h"
#include "Engine/Gameworld/gameworld.h"
#include "Engine/Utils/graphics/frustum.h"
#include "Engine/Utils/random.h"
#include "Engine/_Core/logger.h"
#include "Graphics/GLWrappers/shader.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/global.h"
#include "Graphics/graphics.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/string_cast.hpp"
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

        struct CameraMatrix {
            glm::mat4 view;
            glm::mat4 projection;
        };

        /**
         * @brief Compute matrices correponding to the minimum bounding volume for the light.
         */
        CameraMatrix getLightCameraMatrix(Saga::Camera& camera, glm::vec3 lightDir);
    }

    void shadowMapSetup(std::shared_ptr<GameWorld> world) {
        using namespace GraphicsEngine::Global;
        if (graphics.getFramebuffer("shadowMap")) {
            SWARN("Shadow map already set up. Ignore function.");
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
        drawSystemData->shadowMap = shadowMapFBO->createAndAttachDepthTexture(GL_TEXTURE2, 
                GL_NEAREST, GL_CLAMP_TO_BORDER);
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

            glm::vec3 lightDir = glm::normalize(mainLight.value()->light->getDir());

            auto [lightView, lightProjection] = getLightCameraMatrix(camera, lightDir);
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
        if (ImGui::CollapsingHeader("Shadow Map")) {
            ImGui::SliderFloat("near_plane", &near_plane, 0.01f, 20.0f);
            ImGui::SliderFloat("far_plane", &far_plane, 5.0f, 100.0f);
            ImGui::SliderFloat("mapped_area", &mapped_area, 1.0f, 20.0f);
            ImGui::SliderFloat("distance_to_camera", &distance_to_camera, 1.0f, 50.0f);
        }
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

        CameraMatrix getLightCameraMatrix(Saga::Camera& camera, glm::vec3 lightDir) {
            /* STRACE("light direction: %s", glm::to_string(lightDir).c_str()); */
            glm::vec3 up = glm::vec3(0,1,0);
            // might cause discontinueties
            if (std::abs(glm::dot(up, lightDir) - 1) < 0.0001)
                up = glm::normalize(glm::vec3(.1,0.9,.1));

            // light view at origin

            std::optional<Geometry::Frustum> frustum = camera.getFrustum();
            if (!frustum) {
                SERROR("Camera frustum can't be computed");
                return {
                    glm::mat4(1),
                    glm::mat4(1)
                };
            }

            Geometry::Frustum cameraFrustum_WS = frustum.value();

            auto lightView = glm::lookAt(camera.camera->getPos(), camera.camera->getPos() + lightDir, up);

            // grab frustrum corners, transform to light space
            Geometry::Frustum cameraFrustum_LS = cameraFrustum_WS;
            cameraFrustum_LS.transform(lightView);
            cameraFrustum_LS.homogenize();

            // get bounding box
            BoundingBox cameraBoundingBox_LS = cameraFrustum_LS.getBounds();

            // confine the box
            glm::vec3 maxSize(50, 50, 50);
            BoundingBox confiner {
                .bounds = {
                    - maxSize / 2.0f,
                    + maxSize / 2.0f
                }
            };

            cameraBoundingBox_LS = BoundingBox::confineBox(
                cameraBoundingBox_LS,
                confiner
            );

            // get world space camera position
            glm::vec4 cameraPosWorldSpace = glm::inverse(lightView) * glm::vec4(cameraBoundingBox_LS.centroid(),1);
            cameraPosWorldSpace /= cameraPosWorldSpace.w;

            lightView = glm::lookAt(glm::vec3(cameraPosWorldSpace), glm::vec3(cameraPosWorldSpace) + lightDir, up);

            cameraFrustum_LS = cameraFrustum_WS;
            cameraFrustum_LS.transform(lightView);
            cameraFrustum_LS.homogenize();

            cameraBoundingBox_LS = cameraFrustum_LS.getBounds();

            cameraBoundingBox_LS = BoundingBox::confineBox(
                cameraBoundingBox_LS,
                confiner
            );


            STRACE("Bounds x:[%f, %f] y:[%f, %f] z:[%f, %f]", 
                cameraBoundingBox_LS.bounds[0].x,
                cameraBoundingBox_LS.bounds[1].x,
                cameraBoundingBox_LS.bounds[0].y,
                cameraBoundingBox_LS.bounds[1].y,
                cameraBoundingBox_LS.bounds[0].z,
                cameraBoundingBox_LS.bounds[1].z
            );

            const glm::mat4 lightProjection = glm::ortho(
                cameraBoundingBox_LS.bounds[0].x, cameraBoundingBox_LS.bounds[1].x,
                cameraBoundingBox_LS.bounds[0].y, cameraBoundingBox_LS.bounds[1].y,
                cameraBoundingBox_LS.bounds[0].z, cameraBoundingBox_LS.bounds[1].z
            );

            return {
                lightView,
                lightProjection
            };
        }
    }
}
