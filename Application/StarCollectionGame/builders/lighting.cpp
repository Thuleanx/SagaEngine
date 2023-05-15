#include "lighting.h"
#include "Engine/Components/light.h"
#include "Engine/Components/transform.h"
#include "Engine/Entity/entity.h"
#include "Engine/Gameworld/gameworld.h"

namespace Star {

void createDirectionalLight(std::shared_ptr<Saga::GameWorld> world, Saga::Entity entity, glm::vec3 direction, glm::vec3 color) {
    world->emplace<Saga::Light>(entity, direction, color);
    world->emplace<Saga::Transform>(entity);
}

}
