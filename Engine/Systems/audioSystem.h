#pragma once
#include <memory>

namespace Saga {
class GameWorld;
}

namespace Saga::Systems {
	void audioBankLoader(std::shared_ptr<GameWorld> world);
	void audioBankUnloader(std::shared_ptr<GameWorld> world);

	void audioEmitterAwake(std::shared_ptr<GameWorld> world);
	void audioEmitterUnload(std::shared_ptr<GameWorld> world);

	void registerAudioSystem(std::shared_ptr<GameWorld> world);
};