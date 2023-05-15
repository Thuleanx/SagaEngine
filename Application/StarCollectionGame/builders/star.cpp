#include "star.h"
#include "Application/StarCollectionGame/ECS/star/star.h"
#include "Application/StarCollectionGame/config.h"
#include "Engine/Components/collider.h"
#include "Engine/Components/material.h"
#include "Engine/Components/mesh.h"
#include "Engine/Components/transform.h"
#include "Engine/Systems/events.h"

namespace Star {

Saga::Entity createStar(std::shared_ptr<Saga::GameWorld> world, glm::vec3 pos) {
    Saga::Entity entity = world->createEntity();

    world->emplace<Saga::Mesh>(entity, "Resources/Meshes/star.obj");
    /* world->emplace<Saga::Mesh>(entity, Saga::Mesh::StandardType::Sphere); */
    glm::vec3 starColor = palette.getColor(starColorIndex);

    world->emplace<Saga::Material>(entity, starColor)->material
        ->setEmission(starColor * 1.0f);
    world->emplace<Saga::Transform>(entity)->transform->setPos(pos)->setScale(0.5);

    world->emplace<Saga::Collider>(entity);
    world->emplace<Saga::CylinderCollider>(entity, 1, 0.5); // height, radius

    world->emplace<Star::Comet>(entity, Comet {
        .initialPos = pos,
        .rotationSpeed = 3,
        .boppingSpeed = 2,
        .boppingDistance = 1,
    });
    world->getSystems().addEventSystem(Saga::EngineEvents::OnCollision, entity,
        Saga::System<Saga::Entity, Saga::Entity>(Star::Systems::starCollect));

    return entity;
}

}
