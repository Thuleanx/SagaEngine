#pragma once
#include "Engine/Components/camera.h"
#include <memory>

namespace Saga {
    class GameWorld;
}

namespace Saga::Systems {
    /**
     * @brief Responsible for updating / simulating particle collections in the scene.
     *
     * @ingroup system
     * @param world
     * @param deltaTime time since last frame.
     * @param time time ellapsed since the beginnning of the program.
     */
    void particleSystemSimulationUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time);

    /**
     * @brief Responsible for emitting new particles according to the world's ParticleEmitter 
     *
     * @ingroup system
     * @param world
     * @param deltaTime time since last frame.
     * @param time time ellapsed since the beginnning of the program.
     */
    void particleSystemEmissionUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time);

    /**
     * @brief Responsible for rendering particle systems on screen.
     *
     * @note Despite being a system, you don't really want this to be attached onto a game world to be 
     * automatically called on draw. Instead, this should be part of your draw system's pipeline.
     * Saga::drawSystem should automatically calls it.
     *
     * @ingroup system
     * @param world
     * @param camera the camera to render from.
     */
    void particleSystemOnRender(std::shared_ptr<GameWorld> world, Saga::Camera& camera);

    /**
     * @brief Register Groups that the particle system uses, as well as any of its systems.
     *
     * @param world
     */
    void registerParticleSystem(std::shared_ptr<GameWorld> world);
}
