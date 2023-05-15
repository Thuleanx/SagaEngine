#include "player.h"
#include "Application/StarCollectionGame/ECS/player/player.h"
#include "Application/StarCollectionGame/ECS/star/star.h"
#include "Engine/Components/camera.h"
#include "Engine/Components/collider.h"
#include "Engine/Components/material.h"
#include "Engine/Components/mesh.h"
#include "Engine/Components/rigidbody.h"
#include "Engine/Components/transform.h"
#include "Engine/Entity/entity.h"
#include "Engine/Systems/events.h"

namespace Star {

void createPlayer(std::shared_ptr<Saga::GameWorld> world, Saga::Entity entity, glm::vec3 pos) {
    world->emplace<Star::PlayerInput>(entity);
    world->emplace<Star::Player>(entity, Star::Player {
        .baseMoveSpeed = 5,
        .baseAccelerationSpeed = 20,
        .gravity = 10,
        .jumpSpeed = 10,
        .numStarsCollected = 0,
        .minY = -40,
        .maxY = 100
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

    world->emplace<Saga::Transform>(entity)->transform->setPos(pos);

    world->emplace<Saga::Collider>(entity);
    world->emplace<Saga::CylinderCollider>(entity, 1, 0.5);
    world->emplace<Saga::EllipsoidCollider>(entity, glm::vec3(0.5f));
    world->emplace<Saga::RigidBody>(entity);

    world->emplace<Saga::Material>(entity, glm::vec3(1,1,1));
    world->emplace<Saga::Mesh>(entity, Saga::Mesh::StandardType::Sphere);

    world->getSystems().addEventSystem(Saga::EngineEvents::OnCollision, entity,
        Saga::System<Saga::Entity, Saga::Entity>(Systems::playerGrowth));
}

}
