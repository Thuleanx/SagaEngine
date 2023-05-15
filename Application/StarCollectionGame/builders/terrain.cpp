#include "terrain.h"
#include "Engine/Components/collider.h"
#include "Engine/Components/material.h"
#include "Engine/Components/mesh.h"
#include "Engine/Components/transform.h"

namespace Star {

void createMainStage(std::shared_ptr<Saga::GameWorld> world, Saga::Entity entity, glm::vec3 pos) {
    world->emplace<Saga::Mesh>(entity, "Resources/Meshes/arena.obj");
    world->emplace<Saga::Material>(entity, glm::vec3(.2, 1, 1));
    world->emplace<Saga::Transform>(entity)->transform->setPos(pos);

    world->emplace<Saga::Collider>(entity);
    world->emplace<Saga::MeshCollider>(entity);
}

}
