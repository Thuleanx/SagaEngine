#pragma once

#include "Engine/Entity/entity.h"
#include "Engine/Gameworld/gameworld.h"
#include <glm/glm.hpp>

namespace Star {

Saga::Entity createMainStage(std::shared_ptr<Saga::GameWorld> world, glm::vec3 pos);
Saga::Entity createSubStage(std::shared_ptr<Saga::GameWorld> world, glm::vec3 pos);

}
