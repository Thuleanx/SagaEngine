#pragma once

#include <memory>
#include "Engine/Entity/entity.h"

namespace Saga {
    class GameWorld;
};

namespace Platformer::Systems {
	void friendControllerSystem(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time);
	void registerFriendControllerSystem(std::shared_ptr<Saga::GameWorld> world);
};
