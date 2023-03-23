#pragma once
#include <memory>

namespace Saga {
class GameWorld;
}

namespace Saga::Systems {
	void audioEmitterAwake(std::shared_ptr<GameWorld> world);
	void audioEmitterUpdate(std::shared_ptr<GameWorld> world, float deltaTime, float time);
	void audioEmitterUnload(std::shared_ptr<GameWorld> world);
	void audioCameraPositioning3D(std::shared_ptr<GameWorld> world);
	void registerAudioSystem(std::shared_ptr<GameWorld> world);
	void setupAudioSystem(std::shared_ptr<GameWorld> world);
};