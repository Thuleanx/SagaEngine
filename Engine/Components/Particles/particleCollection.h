#pragma once

#include "Engine/Systems/particleSystem.h"
#include <glm/glm.hpp>
#include <vector>


// Forward declare shader so we don't have to include it.
namespace GraphicsEngine {
    class Shader;
    class VAO;
    class VBO;
    class VEO;
}

namespace Saga {

/**
 * @brief Template for creating a 2D particle.
 */
struct ParticleTemplate {
    glm::vec3   position = glm::vec3(0,0,0), //!< world space position of particle.
                velocity = glm::vec3(0,0,0); //!< world space velocity of particle.
    glm::vec4 color;      //!< color of the particle. (1,1,1,1) is full white.

    float size = 1; // size of particle in world space
    float rotation = 0.0f; //!< rotation of the particle, counterclockwise in radians, relative to the camera.
    float lifetime = 1.0f; //!< lifetime of the particle, in seconds.
};

/**
 * @brief
 * Collection of particles identical to particle systems in other
 * game engines. Allow users to emit arbitrary camera-facing 2D particles.
 * Currently only support simulation on the CPU.
 */
class ParticleCollection {
public:
    /*
     * @brief Default constructor needed for the purpose of populating particle collections in a game world.
     * Do no call this constructor.
     */
    ParticleCollection();

    /**
     * @brief Create an empty collection of particles.
     *
     * @note If the simulation ever exceed this number of particles,
     * the older particles will be deleted when creating more particles.
     */
    ParticleCollection(int maxParticles);

    /**
     * @brief Emit a single particle.
     */
    void emit(ParticleTemplate &particleTemplate);
private:
    /**
     * @brief Internal storage for a particle used in simulating and displaying said particle.
     */
    struct Particle {
        glm::vec3 position, //!< world space position of particle.
            velocity; //!< world space velocity of particle.

        glm::vec4 color; //!< color of the particle.

        float size; //!< world space size.
        float rotation = 0.0f; //!< rotation of the particle, counterclockwise in radians, relative to the camera.
        float lifetime = 1.0f; //!< lifetime of the particle, in seconds.
        float lifetimeRemaining = 0.0f; //!< time left before the particle expires.
    };

    /**
     * @brief Used for rendering the particles. The shader should expect the following uniforms and data:
     */
    std::shared_ptr<GraphicsEngine::Shader> shader;

    /**
     * @brief We use this pool of particles to store all particles.
     * Not only do this allow for faster traversal and rendering (since these particles are contiguous in memory),
     * it also prevents us having to do memory allocation, which is a big no no.
     */
    std::vector<Particle> pool;

    /**
     * @brief We use these two as pointers into the pool of particles, essentially
     * modelling it as a deque. Only particles between leftParticle and rightParticle are active.
     * In between updates, it's guarantee that all particles in between left and right particle
     * are active. rightOfPool in this case is exclusive.
     */
    int leftOfPool = 0,
        rightOfPool = 0;

    std::shared_ptr<GraphicsEngine::VAO> VAO; //!< for drawing particles
    std::shared_ptr<GraphicsEngine::VBO> VBO; //!< for drawing particles
    std::shared_ptr<GraphicsEngine::VEO> VEO; //!< for drawing particles

    /**
     * @brief Get the next index in the pool.
     *
     * @param index the current index.
     * @return index the next index, wrapping around to 0 if neccessary.
     */
    int nextIndex(int index);

    // declare the friend systems so they can use the class's private variables.
    friend void Systems::particleSystemSimulationUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time);
    friend void Systems::particleSystemEmissionUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time);
    friend void Systems::particleSystemOnRender(std::shared_ptr<GameWorld> world, Saga::Camera& camera);
};

}
