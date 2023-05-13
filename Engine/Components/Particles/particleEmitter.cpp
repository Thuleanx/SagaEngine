#include "particleEmitter.h"

namespace Saga {
    ParticleEmitter::ParticleEmitter(float emissionRate, const ParticleTemplate& particleTemplate) :
        emissionRate(emissionRate), particleTemplate(particleTemplate) {}
}
