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
#include <memory>

namespace Saga::Systems {

void particleSystemSimulationUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time) {
    // in the process of simulating the particles, we need
    // to ensure that all and only live particles stay in the index range [left, right)
    for (Saga::ParticleCollection& collection : *world->viewAll<ParticleCollection>()) {
        for (int poolIndex = collection.leftOfPool; poolIndex != collection.rightOfPool; poolIndex = collection.nextIndex(poolIndex)) {
            Saga::ParticleCollection::Particle& particle = collection.pool[poolIndex];

            // standard eulerian simulation
            particle.position += particle.velocity * deltaTime;
            particle.lifetimeRemaining -= deltaTime;

            if (particle.lifetimeRemaining >= 0) continue;

            // if dead, we need to remove this from the index range [left, right)
            // one way is to swap it with the element at left
            if (poolIndex != collection.leftOfPool) {
                // TODO: write specialize code for a faster swap
                std::swap(collection.pool[poolIndex], collection.pool[collection.leftOfPool]);
            }
            collection.leftOfPool = collection.nextIndex(collection.leftOfPool);
        }
    }
}

void particleSystemEmissionUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time) {
    auto group = *world->viewGroup<Saga::ParticleCollection, Saga::ParticleEmitter, Saga::Transform>();
    for (auto [entity, collection, emitter, transform] : group) {
        bool hasTranslated = false;

        int emissionCount = (time - emitter->timeLastEmitted) * emitter->emissionRate;
        if (emissionCount) {
            hasTranslated = true;
            emitter->timeLastEmitted += emissionCount * 1.0f / emitter->emissionRate;
            // here we want to emit at the particle emitter's position. Hence
            // we are adding the transform position to the particle template
            // before emitting
            emitter->particleTemplate.position += transform->getPos();
        }

        while (emissionCount --> 0) collection->emit(emitter->particleTemplate);

        // we undo the add so that the emitter's particleTemplate position
        // is accurate. If we want to be numerically precise, we could have
        // cached the position to store back later, but who cares.
        if (hasTranslated) emitter->particleTemplate.position -= transform->getPos();
    }
}

void particleSystemOnRender(std::shared_ptr<GameWorld> world, Saga::Camera& camera) {
    // switch to additive blend mode
    /* glBlendFunc(GL_SRC_ALPHA, GL_ONE); */
    glDisable(GL_CULL_FACE);
    for (Saga::ParticleCollection& collection : *world->viewAll<ParticleCollection>()) {

        if (!collection.VAO || !collection.VBO) {
            std::vector<float> particleVBO = {
                -0.5f, -0.5f, 0,
                0.5f, -0.5f, 0,
                0.5f, 0.5f, 0,
                -0.5f, 0.5f, 0
            };
            std::vector<int> particleVEO = {
                0, 1, 2, 2, 3, 0
            };
            collection.VBO = std::make_shared<GraphicsEngine::VBO>(particleVBO);
            collection.VEO = std::make_shared<GraphicsEngine::VEO>(particleVEO);
            collection.VAO = std::make_shared<GraphicsEngine::VAO>(collection.VBO,
                             GraphicsEngine::VAOAttrib::POS, collection.VEO);
        }

        if (!collection.shader) {
            GraphicsEngine::Global::graphics.addShader("particleDefault",
            {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
            {"Resources/Shaders/particles/vertex.vert", "Resources/Shaders/particles/particle.frag"});

            collection.shader = GraphicsEngine::Global::graphics.getShader("particleDefault");
        }

        collection.shader->bind();
        /* if (collection.leftOfPool != collection.rightOfPool) { */
        /*     STRACE("rendering particle system: [%d, %d)", collection.leftOfPool, collection.rightOfPool); */
        /* } */
        for (int poolIndex = collection.leftOfPool; poolIndex != collection.rightOfPool; poolIndex = collection.nextIndex(poolIndex)) {
            Saga::ParticleCollection::Particle& particle = collection.pool[poolIndex];

            glm::mat4 rot = glm::inverse(camera.camera->getView());
            rot[3] = glm::vec4(0,0,0,1);

            glm::mat4 mvp = camera.camera->getProjection() * camera.camera->getView();
            mvp = glm::translate(mvp, particle.position);
            mvp = mvp * rot;
            mvp = mvp * glm::scale(glm::mat4(1), glm::vec3(particle.size));

            collection.shader->setVec4("color", particle.color);
            collection.shader->setMat4("mvp", mvp);

            collection.VAO->draw();
        }
        collection.shader->unbind();
    }
    glEnable(GL_CULL_FACE);
    /* glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); */
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
