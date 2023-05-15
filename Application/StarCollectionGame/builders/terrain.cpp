#include "terrain.h"
#include "Engine/Components/collider.h"
#include "Engine/Components/material.h"
#include "Engine/Components/mesh.h"
#include "Engine/Components/transform.h"

namespace Star {

Saga::Entity createMainStage(std::shared_ptr<Saga::GameWorld> world, glm::vec3 pos) {
    Saga::Entity entity = world->createEntity();
    world->emplace<Saga::Mesh>(entity, "Resources/Meshes/arena.obj");
    world->emplace<Saga::Material>(entity, glm::vec3(.2, 1, 1), 0);
    world->emplace<Saga::Transform>(entity)->transform->setPos(pos);

    world->emplace<Saga::Collider>(entity);
    world->emplace<Saga::MeshCollider>(entity);
    return entity;
}

Saga::Entity createSubStage(std::shared_ptr<Saga::GameWorld> world, glm::vec3 pos) {
    Saga::Entity entity = world->createEntity();
    world->emplace<Saga::Mesh>(entity, "Resources/Meshes/arena_rock.obj");
    world->emplace<Saga::Material>(entity, glm::vec3(.2, 1, 1), 0);
    world->emplace<Saga::Transform>(entity)->transform->setPos(pos);

    world->emplace<Saga::Collider>(entity);
    world->emplace<Saga::MeshCollider>(entity);
    return entity;
}

}
