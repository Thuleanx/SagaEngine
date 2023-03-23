#pragma once
#include <memory>

namespace Saga {
class GameWorld;
}

namespace Saga::Systems {
	/**
	 * @brief Preprocess all audio event emitters. This loads their events's sample data if requested, and additionally play them if AudioEmitter::playOnAwake is true.
	 * @ingroup system
	 * @param world 
	 */
	void audioEmitterAwake(std::shared_ptr<GameWorld> world);

	/**
	 * @brief Perform updates to audio emitters in the scene. 
	 * Specifically, if the emitter is playing a sound and it's a 3D emitter, makes sure that the 3D positioning of the emitter is updated correctly.
	 * 
	 * @ingroup system
	 * @param world 
	 * @param deltaTime time since last update.
	 * @param time time since start of the program.
	 */
	void audioEmitterUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time);

	/**
	 * @brief Perform cleanup on the event emitters. 
	 * Specifically, this stops and release event instances, as well as unload their sample data if it was previously loaded on awake.
	 * 
	 * @ingroup system
	 * @param world 
	 */
	void audioEmitterUnload(std::shared_ptr<GameWorld> world);

	/**
	 * @brief Adjust the audio listener. By default, this sets it to the first camera in the scene.
	 * 
	 * @ingroup system
	 * @param world 
	 */
	void audioCameraPositioning3D(std::shared_ptr<GameWorld> world);

	/**
	 * @brief Register the audio system.
	 * 
	 * @ingroup system
	 * @param world 
	 */
	void registerAudioSystem(std::shared_ptr<GameWorld> world);

	/**
	 * @brief Setup the audio set of systems, adding the appropriate systems to their own Stages.
	 * 
	 * @ingroup system
	 * @param world 
	 * @see audioEmitterAwake, @see audioEmitterUpdate, @see audioEmitterUnload, @see audioCameraPositioning3D, @see registerAudioSystem
	 */
	void setupAudioSystem(std::shared_ptr<GameWorld> world);
};