#include "particleCollection.h"
#include "Engine/Utils/random.h"
#include "Engine/_Core/logger.h"

namespace Saga {
    ParticleCollection::ParticleCollection() {}

    ParticleCollection::ParticleCollection(int maxParticles) :
        pool(maxParticles) { }

    int ParticleCollection::nextIndex(int index) { return index+1 == pool.size() ? 0 : index+1; }

    void ParticleCollection::emit(ParticleTemplate &particleTemplate) {
        int nextRight = nextIndex(rightOfPool);

        // we take over the position of the first particle
        if (nextRight == leftOfPool)
            leftOfPool = nextIndex(leftOfPool);

        Particle& particle = pool[rightOfPool];
        particle.position = particleTemplate.position;

        particle.velocity = particleTemplate.velocity;

        if (particleTemplate.velocityRandomness) {
            glm::vec3 velocityVariation = glm::vec3(Random::getUniformRandom01(), Random::getUniformRandom01(), Random::getUniformRandom01());
            particle.velocity += (velocityVariation - glm::vec3(0.5)) * particleTemplate.velocityRandomness.value();
        }

        particle.color = particleTemplate.color;

        particle.lifetime = particle.lifetimeRemaining = particleTemplate.lifetime;

        particle.rotation = particleTemplate.rotation;
        particle.size = particleTemplate.size;
        if (particleTemplate.sizeVariation) {
            float sizeVariation = (Random::getUniformRandom01() - 0.5f) * particleTemplate.sizeVariation.value();
            // prevent size from being negative
            particle.size = std::max(particle.size + sizeVariation, 0.0f);
        }

        rightOfPool = nextRight;
        /* STRACE("particle emitted [%d %d] %d", leftOfPool, rightOfPool, pool.size()); */
    }
}
