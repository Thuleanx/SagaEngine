#pragma once

#include <memory>
#include "Engine/Entity/entity.h"

namespace Saga { 
	class GameWorld; 
}

namespace Platformer::Systems {
	void playerControllerSystem(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time);
	void registerPlayerControllerSystem(std::shared_ptr<Saga::GameWorld> world);
}