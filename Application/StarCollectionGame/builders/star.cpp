#include "star.h"
#include "Application/StarCollectionGame/ECS/star/star.h"
#include "Engine/Components/collider.h"
#include "Engine/Components/material.h"
#include "Engine/Components/mesh.h"
#include "Engine/Components/transform.h"
#include "Engine/Systems/events.h"

namespace Star {

void createStar(std::shared_ptr<Saga::GameWorld> world, Saga::Entity entity, glm::vec3 pos) {
    world->emplace<Saga::Material>(entity, glm::vec3(2,2,2));
    world->emplace<Saga::Mesh>(entity, Saga::Mesh::StandardType::Sphere);
    world->emplace<Saga::Transform>(entity)->transform->setPos(pos);

    world->emplace<Saga::Collider>(entity);
    world->emplace<Saga::CylinderCollider>(entity, 1, 0.5); // height, radius

    world->getSystems().addEventSystem(Saga::EngineEvents::OnCollision, entity, Saga::System<Saga::Entity, Saga::Systems::Collision>(Star::Systems::starCollect));
}

}
