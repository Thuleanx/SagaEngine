#include "player.h"
#include "Application/StarCollectionGame/ECS/player/player.h"
#include "Engine/Components/camera.h"
#include "Engine/Components/collider.h"
#include "Engine/Components/material.h"
#include "Engine/Components/mesh.h"
#include "Engine/Components/rigidbody.h"
#include "Engine/Components/transform.h"

namespace Star {

void createPlayer(std::shared_ptr<Saga::GameWorld> world, Saga::Entity entity) {
    world->emplace<Star::PlayerInput>(entity);
    world->emplace<Star::Player>(entity, Star::Player {
        .movementSpeed = 3,
        .accelerationSpeed = 3,
        .gravity = 2,
        .jumpSpeed = 10,
        .numStarsCollected = 0
    });
    world->emplace<Star::Camera>(entity, Star::Camera {
        .focusOffset = glm::vec3(0, 1, 1),
        .shoulderOffset = glm::vec3(0, .25f, 0),
        .turnRate= glm::vec2(1.00f/50, 1.00f/50),
        .distance = 3,
        .isInitialFrame = true
    });

    std::shared_ptr<GraphicsEngine::Camera> defaultCamera 
        = std::make_shared<GraphicsEngine::Camera>();

    world->emplace<Saga::Camera>(entity, Saga::Camera {
        .camera = defaultCamera,
        .clearColorBufferOnDraw = true,
        .clearColor = glm::vec3(0,0,0),
        .globalShaderCoefs = glm::vec3(0.2, 1, 1),
        .shader = "phong"
    });

    world->emplace<Saga::Transform>(entity)->transform->setPos(glm::vec3(0,0,0));

    world->emplace<Saga::Collider>(entity);
    world->emplace<Saga::EllipsoidCollider>(entity, glm::vec3(0.5f));
    world->emplace<Saga::RigidBody>(entity);

    world->emplace<Saga::Material>(entity, glm::vec3(1,1,1));
    world->emplace<Saga::Mesh>(entity, Saga::Mesh::StandardType::Sphere);
}

}
