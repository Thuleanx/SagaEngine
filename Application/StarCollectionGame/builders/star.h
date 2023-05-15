#pragma once

#include "Engine/Entity/entity.h"
#include "Engine/Gameworld/gameworld.h"

namespace Star {
    void createStar(std::shared_ptr<Saga::GameWorld> world, Saga::Entity entity);
}
