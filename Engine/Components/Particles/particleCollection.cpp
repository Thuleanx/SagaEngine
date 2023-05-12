#include "particleCollection.h"

namespace Saga {
    ParticleCollection::ParticleCollection() {}

    ParticleCollection::ParticleCollection(int maxParticles) :
        pool(maxParticles) { }

    int ParticleCollection::nextIndex(int index) { return index+1 == pool.size() ? 0 : index; }

    void ParticleCollection::emit(ParticleTemplate &particleTemplate) {
        int nextRight = nextIndex(rightOfPool);

        // we take over the position of the first particle
        if (nextRight == leftOfPool)
            leftOfPool = nextIndex(leftOfPool);

        Particle& particle = pool[rightOfPool];
        particle.position = particleTemplate.position;
        particle.velocity = particleTemplate.velocity;
        particle.color = particleTemplate.color;
        particle.lifetime = particle.lifetimeRemaining = particleTemplate.lifetime;
        particle.rotation = particleTemplate.rotation;

        rightOfPool = nextRight;
    }
}