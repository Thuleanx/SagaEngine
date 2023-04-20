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
    virtual void update(float deltaTime, float time) override;

	std::shared_ptr<Saga::GameWorld> mainWorld;
	std::shared_ptr<Saga::AudioEventInstance> loopEvent;
};

}
