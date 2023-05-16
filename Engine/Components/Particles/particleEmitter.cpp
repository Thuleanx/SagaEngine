#include "particleEmitter.h"

namespace Saga {
    ParticleEmitter::ParticleEmitter(float emissionRate, const ParticleTemplate& particleTemplate) :
        emissionRate(emissionRate), particleTemplate(particleTemplate) {}

    bool ParticleEmitter::isPlaying() {
        return playing;
    }

    void ParticleEmitter::play() {
        shouldBurst = true;
        playing = true;
    }

    void ParticleEmitter::stop() {
        shouldBurst = playing = false;
    }
}
