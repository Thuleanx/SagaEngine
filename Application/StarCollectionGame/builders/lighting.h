#pragma once

#include "Engine/Entity/entity.h"
#include <glm/glm.hpp>
#include <memory>

namespace Saga {
class GameWorld;
}

namespace Star {

void createDirectionalLight(std::shared_ptr<Saga::GameWorld> world, Saga::Entity entity, glm::vec3 direction, glm::vec3 color);

}
