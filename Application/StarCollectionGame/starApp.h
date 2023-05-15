#pragma once

#include "Engine/saga.h"

namespace Star {

class StarApp : public Saga::App {
public:
	StarApp();
	virtual ~StarApp() override;
private:
    void worldSetup();
    void endingWorldSetup();
	std::shared_ptr<Saga::GameWorld> mainWorld;
};

}
