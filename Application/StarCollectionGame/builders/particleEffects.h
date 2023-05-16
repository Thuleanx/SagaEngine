#pragma once

#include "engine/entity/entity.h"
#include <glm/glm.hpp>
#include "Engine/Gameworld/gameworld.h"

namespace Star {

Saga::Entity createStarCollectionParticle(std::shared_ptr<Saga::GameWorld> world, glm::vec3 pos);

}
