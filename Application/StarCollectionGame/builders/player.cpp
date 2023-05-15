#include "player.h"
#include "Application/StarCollectionGame/ECS/player/player.h"
#include "Application/StarCollectionGame/ECS/star/star.h"
#include "Application/StarCollectionGame/config.h"
#include "Engine/Components/camera.h"
#include "Engine/Components/collider.h"
#include "Engine/Components/material.h"
#include "Engine/Components/mesh.h"
#include "Engine/Components/rigidbody.h"
#include "Engine/Components/transform.h"
#include "Engine/Entity/entity.h"
#include "Engine/Systems/events.h"

namespace Star {

Saga::Entity createPlayer(std::shared_ptr<Saga::GameWorld> world, glm::vec3 pos) {
    Saga::Entity entity = world->createEntity();
    world->emplace<Star::PlayerInput>(entity);
    world->emplace<Star::Player>(entity, Star::Player {
        .baseMoveSpeed = 5,
        .baseAccelerationSpeed = 20,
        .baseJumpSpeed = 10,
        .gravity = 10,
        .halfGravityThreshold = 5,
        .minY = -50,
        .maxY = 100
    });
    world->emplace<Star::Camera>(entity, Star::Camera {
        .focusOffset = glm::vec3(0, 1, 1),
        .shoulderOffset = glm::vec3(0, .25f, 0),
        .turnRate= glm::vec2(1.00f/50, 1.00f/50),
        .realDistance = 3,
        .distance = 3,
        .distanceSmoothing = 0.6,
        .positionalSmoothing = 0.1,
        .isInitialFrame = true
    });

    std::shared_ptr<GraphicsEngine::Camera> defaultCamera 
        = std::make_shared<GraphicsEngine::Camera>();

    world->emplace<Saga::Camera>(entity, Saga::Camera {
        .camera = defaultCamera,
        .clearColorBufferOnDraw = true,
        .clearColor = glm::vec3(0,0,0),
        .globalShaderCoefs = glm::vec3(0.5, 1, 1),
        .ambientColor = palette.getColor(shadowColorIndex),
        .shader = "phong"
    });

    world->emplace<Saga::Transform>(entity)->transform->setPos(pos);

    world->emplace<Saga::Collider>(entity);
    world->emplace<Saga::CylinderCollider>(entity, 1, 0.5);
    world->emplace<Saga::EllipsoidCollider>(entity, glm::vec3(0.5f));
    world->emplace<Saga::RigidBody>(entity);

    glm::vec3 playerColor = palette.getColor(playerColorIndex);

    world->emplace<Saga::Material>(entity, playerColor, 0.5);
    world->emplace<Saga::Mesh>(entity, Saga::Mesh::StandardType::Sphere);

    world->getSystems().addEventSystem(Saga::EngineEvents::OnCollision, entity,
        Saga::System<Saga::Entity, Saga::Entity>(Systems::playerGrowth));
    return entity;
}

}
