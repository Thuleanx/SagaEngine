#include "audioSystem.h"
#include "../Components/audioemitter.h"
#include "../Components/transform.h"
#include "../Components/camera.h"
#include "../Components/rigidbody.h"
#include "../GameWorld/gameworld.h"

namespace Saga::Systems {
	void audioEmitterAwake(std::shared_ptr<GameWorld> world) {
		for (auto& [entity, audioEmitter, transform] : *world->viewGroup<AudioEmitter, Transform>()) {
			// release audio instances
			if (audioEmitter->preload) 
				AudioEngine::loadSampleData(audioEmitter->eventName);
			// play on awake
			if (audioEmitter->playOnAwake) 
				audioEmitter->audioInstance = AudioEngine::playEvent(audioEmitter->eventName);
		}
	}

	void audioEmitterUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time) {
		for (auto& [entity, audioEmitter, transform] : *world->viewGroup<AudioEmitter, Transform>()) {
			if (audioEmitter->is3D && audioEmitter->audioInstance) {
				glm::vec3 velocity(0,0,0);

				// use velocity if rigidbody exists
				if (world->hasComponent<RigidBody>(entity)) {
					auto rigidbody = world->getComponent<RigidBody>(entity);
					velocity = rigidbody->velocity;
				}

				AudioEngine::set3DAttributes(audioEmitter->audioInstance, AudioEngine::EventAttributes(
					transform->getPos(),
					velocity,
					transform->getForward(),
					transform->getUp()
				));
			}
		}
	}

	void audioEmitterUnload(std::shared_ptr<GameWorld> world) {
		for (auto& [entity, audioEmitter, transform] : *world->viewGroup<AudioEmitter, Transform>()) {
			// release audio instances
			if (audioEmitter->audioInstance) {
				// remember to unload data 
				if (audioEmitter->preload) AudioEngine::unloadSampleData(audioEmitter->eventName);
				AudioEngine::stopEvent(audioEmitter->audioInstance);
				AudioEngine::releaseEvent(audioEmitter->audioInstance);
				audioEmitter->audioInstance = nullptr;
			}
		}
	}

	void audioCameraPositioning3D(std::shared_ptr<GameWorld> world) {
		// if camera exists
		if (world->viewAll<Camera>()->begin() != world->viewAll<Camera>()->end()) {
			Saga::Camera& camera = *(world->viewAll<Camera>()->begin());

			glm::vec3 velocity(0,0,0);

			Entity cameraEntity = world->getEntity(&camera);

			// use velocity if rigidbody exists
			if (world->hasComponent<RigidBody>(cameraEntity)) {
				auto rigidbody = world->getComponent<RigidBody>( cameraEntity );
				velocity = rigidbody->velocity;
			}

			AudioEngine::setListenerData(AudioEngine::EventAttributes(
				camera.camera->getPos(),
				velocity,
				camera.camera->getLook(),
				camera.camera->getUp()
			));
		}
	}

	void registerAudioSystem(std::shared_ptr<GameWorld> world) {
		world->registerGroup<AudioEmitter, Transform>();
	}

	void setupAudioSystem(std::shared_ptr<GameWorld> world) {
		registerAudioSystem(world);
		world->getSystems().addStagedSystem(std::make_shared<System<>>(audioEmitterAwake), SystemManager::Stage::Awake);
		world->getSystems().addStagedSystem(std::make_shared<System<float, float>>(audioEmitterUpdate), SystemManager::Stage::Update);
		world->getSystems().addStagedSystem(std::make_shared<System<>>(audioEmitterUnload), SystemManager::Stage::Cleanup);
	}
};