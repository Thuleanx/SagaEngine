#pragma once

#include "Engine/Components/Particles/particleCollection.h"
#include "Engine/Systems/particleSystem.h"
#include <glm/glm.hpp>

namespace Saga {

/**
 * @brief Particle emitter responsible for emitting particles.
 */
class ParticleEmitter {
public:
    // default emitter for gameworld, never call.
    ParticleEmitter() = default;
    /**
     * @brief Create new particle emitter.
     */
    ParticleEmitter(float emissionRate, const ParticleTemplate& particleTemplate);

    float emissionRate; //!< in particles per second.
    ParticleTemplate particleTemplate; //!< template for a particle.
private:
    float timeLastEmitted = 0; //!< time particle last emitted in seconds.

    friend void Systems::particleSystemEmissionUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time);
};

}
