#include "particleSystem.h"
#include "Engine/Components/Particles/particleCollection.h"
#include "Engine/Components/Particles/particleEmitter.h"
#include "Engine/Components/camera.h"
#include "Engine/Components/transform.h"
#include "Engine/Gameworld/gameworld.h"
#include "Graphics/GLWrappers/shader.h"
#include "Graphics/GLWrappers/vao.h"
#include "Graphics/GLWrappers/veo.h"
#include "Graphics/global.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include <algorithm>
#include <memory>

namespace Saga::Systems {

void particleSystemSimulationUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time) {
    // in the process of simulating the particles, we need
    // to ensure that all and only live particles stay in the index range [left, right)
    for (Saga::ParticleCollection& collection : *world->viewAll<ParticleCollection>()) {
        collection.sortByLifetime();

        for (int poolIndex = 0; poolIndex < collection.numberOfLiveParticles; poolIndex++) {
            Saga::ParticleCollection::Particle& particle = collection.pool[poolIndex];

            // standard eulerian simulation
            particle.velocity += glm::vec3(0,-1,0) * deltaTime * particle.gravity;
            particle.position += particle.velocity * deltaTime;
            particle.lifetimeRemaining -= deltaTime;
            if (particle.lifetimeRemaining <= 0) {
                collection.numberOfLiveParticles = poolIndex;
                break;
            }
        }
    }
}

void particleSystemEmissionUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time) {
    auto group = *world->viewGroup<Saga::ParticleCollection, Saga::ParticleEmitter, Saga::Transform>();
    for (auto [entity, collection, emitter, transform] : group) {
        if (emitter->isPlaying()) {
            bool hasTranslated = false;

            int rawEmissions = (time - emitter->timeLastEmitted) * emitter->emissionRate;
            if (rawEmissions) emitter->timeLastEmitted += rawEmissions * 1.0f / emitter->emissionRate;

            int emissionCount = rawEmissions + (emitter->shouldBurst ? emitter->burst : 0);
            
            if (emissionCount) {
                hasTranslated = true;
                // here we want to emit at the particle emitter's position. Hence
                // we are adding the transform position to the particle template
                // before emitting
                emitter->particleTemplate.position += transform->getPos();

                // we need to sort the collection so that we know which to override
                collection->sortByLifetime();
                collection->resetOverrideElement();
            }

            while (emissionCount --> 0) collection->emit(emitter->particleTemplate);

            // we undo the add so that the emitter's particleTemplate position
            // is accurate. If we want to be numerically precise, we could have
            // cached the position to store back later, but who cares.
            if (hasTranslated) emitter->particleTemplate.position -= transform->getPos();

            emitter->shouldBurst = false;
        }
    }
}

void particleSystemOnRender(std::shared_ptr<GameWorld> world, Saga::Camera& camera) {
    // switch to additive blend mode
    glDisable(GL_CULL_FACE);
    /* glDepthMask(false); */
    glEnable(GL_BLEND);
    for (Saga::ParticleCollection& collection : *world->viewAll<ParticleCollection>()) {
        // additive blend :>
        switch (collection.blendMode) {
            case ParticleCollection::ADDITIVE:
                glBlendFunc(GL_ONE, GL_ONE);
                break;
            case ParticleCollection::NORMAL:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                collection.sortByDistanceToCamera(camera);
                break;
            case ParticleCollection::MULTIPLICATIVE:
                glBlendFunc(GL_DST_COLOR, GL_ZERO);
                break;
        }

        if (!collection.VAO || !collection.VBO) {
            std::vector<float> particleVBO = {
                -0.5f, -0.5f, 0,
                0.0f, 0.0f,
                0.5f, -0.5f, 0,
                1.0f, 0.0f,
                0.5f, 0.5f, 0,
                1.0f, 1.0f,
                -0.5f, 0.5f, 0,
                0.0f, 1.0f,
            };
            std::vector<int> particleVEO = {
                0, 1, 2, 2, 3, 0
            };
            collection.VBO = std::make_shared<GraphicsEngine::VBO>(particleVBO);
            collection.VEO = std::make_shared<GraphicsEngine::VEO>(particleVEO);
            collection.VAO = std::make_shared<GraphicsEngine::VAO>(collection.VBO,
                             GraphicsEngine::VAOAttrib::POS | GraphicsEngine::VAOAttrib::UV, collection.VEO);
        }

        if (!collection.shader) {
            GraphicsEngine::Global::graphics.addShader("particleDefault",
            {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
            {"Resources/Shaders/particles/vertex.vert", "Resources/Shaders/particles/particleTextured.frag"});

            collection.shader = GraphicsEngine::Global::graphics.getShader("particleDefault");
        }

        collection.shader->bind();
        if (collection.mainTex) collection.mainTex->bind(GL_TEXTURE0);


        for (int poolIndex = 0; poolIndex < collection.numberOfLiveParticles; poolIndex++) {
            Saga::ParticleCollection::Particle& particle = collection.pool[poolIndex];

            glm::mat4 rot = glm::mat4(glm::mat3(camera.camera->getView()));
            rot[3][3] = 1;
            rot = glm::inverse(rot);

            glm::mat4 mvp = camera.camera->getProjection() * camera.camera->getView();
            mvp = glm::translate(mvp, particle.position);
            mvp = mvp * rot;
            mvp = mvp * glm::scale(glm::mat4(1), glm::vec3(particle.size));

            collection.shader->setVec4("color", particle.color);
            collection.shader->setMat4("mvp", mvp);
            collection.shader->setSampler("MainTex", 0);

            collection.VAO->draw();
        }

        if (collection.mainTex) collection.mainTex->unbind(GL_TEXTURE0);
        collection.shader->unbind();
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /* glDepthMask(true); */
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void registerParticleSystem(std::shared_ptr<GameWorld> world) {
    world->registerGroup<Saga::ParticleCollection, Saga::ParticleEmitter, Saga::Transform>();
    // register the systems
    world->getSystems().addStagedSystem(Saga::System<float, float>(particleSystemSimulationUpdate),
                                        SystemManager::Stage::Update);
    world->getSystems().addStagedSystem(Saga::System<float, float>(particleSystemEmissionUpdate),
                                        SystemManager::Stage::Update);
}
}
