#include "lighting.h"
#include "Engine/Components/light.h"
#include "Engine/Components/transform.h"
#include "Engine/Entity/entity.h"
#include "Engine/Gameworld/gameworld.h"

namespace Star {

Saga::Entity createDirectionalLight(std::shared_ptr<Saga::GameWorld> world, glm::vec3 direction, glm::vec3 color) {
    Saga::Entity entity = world->createEntity();
    world->emplace<Saga::Light>(entity, direction, color);
    world->emplace<Saga::Transform>(entity);
    return entity;
}

}
