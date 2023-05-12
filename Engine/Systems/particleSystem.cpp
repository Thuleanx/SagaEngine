#include "particleSystem.h"
#include "Engine/Components/Particles/particleCollection.h"
#include "Engine/Gameworld/gameworld.h"
#include "Graphics/GLWrappers/shader.h"

namespace Saga::Systems {
    void particleSystemOnUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time) {
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

    void particleSystemOnRender(std::shared_ptr<GameWorld> world) {
        // switch to additive blend mode
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        for (Saga::ParticleCollection& collection : *world->viewAll<ParticleCollection>()) {
            collection.shader->bind();
            for (int poolIndex = collection.leftOfPool; poolIndex != collection.rightOfPool; poolIndex = collection.nextIndex(poolIndex)) {
                Saga::ParticleCollection::Particle& particle = collection.pool[poolIndex];

                collection.shader->setVec4("color", particle.color);
            }
            collection.shader->unbind();
        }
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void registerParticleSystem(std::shared_ptr<GameWorld> world) {
        /* world->registerGroup<Saga::ParticleCollection, Saga::Transform>(); */
        // register the systems
        world->getSystems().addStagedSystem(Saga::System<float, float>(particleSystemOnUpdate), 
                SystemManager::Stage::Update);
    }
}
