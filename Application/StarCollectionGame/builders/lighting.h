#pragma once

#include "Engine/Entity/entity.h"
#include <glm/glm.hpp>
#include <memory>

namespace Saga {
class GameWorld;
}

namespace Star {

Saga::Entity createDirectionalLight(std::shared_ptr<Saga::GameWorld> world, glm::vec3 direction, glm::vec3 color);

}
