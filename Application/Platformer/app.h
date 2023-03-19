#pragma once

#include "Engine/saga.h"

namespace Platformer {

class App : public Saga::App {
public:
	App();
	virtual ~App() override;
private:
	void setupWorld();
	void setupSystems();

	std::shared_ptr<Saga::GameWorld> world;
	std::shared_ptr<Saga::AudioEventInstance> loopEvent;
};

}