#include "particleCollection.h"
#include "Engine/Utils/random.h"
#include "Engine/_Core/logger.h"
#include "Graphics/GLWrappers/texture.h"
#include <glm/gtx/norm.hpp>

namespace Saga {
ParticleCollection::ParticleCollection() {}

ParticleCollection::ParticleCollection(int maxParticles, BlendMode mode, std::shared_ptr<GraphicsEngine::Texture> mainTex) :
    pool(maxParticles), mainTex(mainTex), blendMode(mode) { }

int ParticleCollection::nextIndex(int index) {
    return index+1 == pool.size() ? 0 : index+1;
}

void ParticleCollection::emit(ParticleTemplate &particleTemplate) {
    int currentPos = numberOfLiveParticles == pool.size() ? overrideElement : numberOfLiveParticles;

    // we take over the position of the first particle
    Particle& particle = pool[currentPos];
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

    numberOfLiveParticles = std::max<int>(numberOfLiveParticles+1, pool.size());
    overrideElement = nextIndex(overrideElement);
    /* STRACE("particle emitted [%d %d] %d", leftOfPool, rightOfPool, pool.size()); */
}

void ParticleCollection::resetOverrideElement() {
    overrideElement = 0;
}

void ParticleCollection::sortByLifetime() {
    if (!numberOfLiveParticles) return;

    auto sortByLifetime = [](const ParticleCollection::Particle& p0, const ParticleCollection::Particle& p1) -> bool {
        return p0.lifetimeRemaining > p1.lifetimeRemaining;
    };

    std::sort(pool.begin(), pool.begin() + numberOfLiveParticles, sortByLifetime);
}

void ParticleCollection::sortByDistanceToCamera(Saga::Camera& camera) {
    if (!numberOfLiveParticles) return;

    auto sortByCameraDistance =
    [&camera](const ParticleCollection::Particle& p0, const ParticleCollection::Particle& p1) -> bool {
        // length2 so we don't waste compute on doing sqrt for no reason
        float t0 = glm::dot(camera.camera->getLook(), p0.position);
        float t1 = glm::dot(camera.camera->getLook(), p1.position);
        return t0 > t1;
    };

    std::sort(pool.begin(), pool.begin() + numberOfLiveParticles, sortByCameraDistance);
}

void ParticleCollection::reverseOrder() {
    if (!numberOfLiveParticles) return;
    std::reverse(pool.begin(), pool.end() + numberOfLiveParticles);
}
}
