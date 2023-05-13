#pragma once

#include "Engine/Components/camera.h"
#include "Engine/Systems/particleSystem.h"
#include <glm/glm.hpp>
#include <vector>


// Forward declare shader so we don't have to include it.
namespace GraphicsEngine {
    class Shader;
    class VAO;
    class VBO;
    class VEO;
    class Texture;
}

namespace Saga {

/**
 * @brief Template for creating a 2D particle.
 */
struct ParticleTemplate {
    glm::vec3   position = glm::vec3(0,0,0); //!< world space position of particle.
    
    glm::vec3 velocity = glm::vec3(0,0,0); //!< world space velocity of particle.
    std::optional<glm::vec3> velocityRandomness; // randomness to velocity applied to every spawn.
    
    glm::vec4 color;      //!< color of the particle. (1,1,1,1) is full white.

    float size = 1; // size of particle in world space
    std::optional<float> sizeVariation; //!< size parameters

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
    /**
    * @brief Different particle blend modes that's supported.
    */
    enum BlendMode {
        ADDITIVE = 0, //!< a + b
        NORMAL = 1, //!< a(srcAlpha) * (1 - srcAlpha)b
        MULTIPLICATIVE = 2, //!< ab
    };

    /*
     * @brief Default constructor needed for the purpose of populating particle collections in a game world.
     * Do no call this constructor.
     */
    ParticleCollection();

    /**
     * @brief Create an empty collection of particles.
     *
     * @param maxParticles the maximum number of particles in the collection. 
     * @param blendMode how the particles should blend
     * @param mainTex the main texture used for this particle system.
     *
     * @note If the simulation ever exceed this number of particles,
     * the older particles will be deleted when creating more particles.
     */
    ParticleCollection(int maxParticles, BlendMode blendMode = NORMAL, std::shared_ptr<GraphicsEngine::Texture> mainTex = nullptr);

    /**
     * @brief Emit a single particle.
     */
    void emit(ParticleTemplate &particleTemplate);

    /**
     * @brief Sort collection by lifetime remaining. This is useful in simulating the particles (as you will know exactly when to stop), as well 
     * as emitting particles (to know which particle to override).
     */
    void sortByLifetime();

    /**
     * @brief Sort collection by distance to camera. Further particles are earlier in the list of particles.
     * This allows for z sorting of the particles before rendering.
     */
    void sortByDistanceToCamera(Saga::Camera& camera);

    /**
     * @brief Reverse order of the particles in the pool.
     */
    void reverseOrder();

    /**
     * @brief Reset which element to override when emitting to 0.
     */
    void resetOverrideElement();
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

    int overrideElement = 0; //!< pointer to the element to override when emitting more particles than maximum.
                             // This pointer increments and loops around.

    int numberOfLiveParticles = 0; //!< number of particles whose lifetime is positive.

    std::shared_ptr<GraphicsEngine::VAO> VAO; //!< for drawing particles.
    std::shared_ptr<GraphicsEngine::VBO> VBO; //!< for drawing particles.
    std::shared_ptr<GraphicsEngine::VEO> VEO; //!< for drawing particles.
    std::shared_ptr<GraphicsEngine::Texture> mainTex; //!< for textured particles.

    BlendMode blendMode; //!< used for when rendering the particles. Tells particles how they should blend.

    /**
     * @brief Get the next index in the pool, wrapping around to 0 at pool size.
     *
     * @param index the current index.
     * @return index the next index, wrapping around to 0 if neccessary.
     */
    int nextIndex(int index);


    // declare the friend systems so they can use the class's private variables.
    friend void Systems::particleSystemSimulationUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time);
    friend void Systems::particleSystemOnRender(std::shared_ptr<GameWorld> world, Saga::Camera& camera);
};

}
