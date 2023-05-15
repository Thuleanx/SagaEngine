#pragma once

#include "Engine/Entity/entity.h"
#include "Engine/Systems/collisionSystem.h"
#include <memory>

namespace Saga {
class GameWorld;
}

namespace Star::Systems {

void starCollect(std::shared_ptr<Saga::GameWorld> world, Saga::Entity entity, Saga::Entity other);

}
