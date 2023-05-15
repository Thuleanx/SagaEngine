#pragma once

#include "Engine/Entity/entity.h"
#include "Engine/Systems/collisionSystem.h"
#include <memory>

namespace Saga {
class GameWorld;
}

namespace Star {
struct Collectible {
};
}

namespace Star::Systems {

void starCollect(std::shared_ptr<Saga::GameWorld> world, Saga::Entity star, Saga::Entity other);
void playerGrowth(std::shared_ptr<Saga::GameWorld> world, Saga::Entity player, Saga::Entity other);

}
