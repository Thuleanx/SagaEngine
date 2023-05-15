#pragma once

#include "Engine/Entity/entity.h"
#include "Engine/Gameworld/gameworld.h"
#include <glm/glm.hpp>

namespace Star {

void createMainStage(std::shared_ptr<Saga::GameWorld> world, Saga::Entity entity, glm::vec3 pos);

}
