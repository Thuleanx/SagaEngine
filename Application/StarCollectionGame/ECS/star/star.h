#pragma once

#include "Engine/Entity/entity.h"
#include "Engine/Systems/collisionSystem.h"
#include <memory>

namespace Saga {
class GameWorld;
}

namespace Star {
struct Comet {
    glm::vec3 initialPos;

    float rotationSpeed;
    float boppingSpeed;
    float boppingDistance;

    Saga::Entity effect;
};
}

namespace Star::Systems {

void animateStar(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time);
void starCollect(std::shared_ptr<Saga::GameWorld> world, Saga::Entity star, Saga::Entity other);
void playerGrowth(std::shared_ptr<Saga::GameWorld> world, Saga::Entity player, Saga::Entity other);

}
