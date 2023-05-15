#include "player.h"
#include "Application/StarCollectionGame/ECS/editor.h"
#include "Application/StarCollectionGame/ECS/star/star.h"
#include "Engine/Components/camera.h"
#include "Engine/Components/collider.h"
#include "Engine/Components/rigidbody.h"
#include "Engine/Components/transform.h"
#include "Engine/Gameworld/gameworld.h"
#include "Engine/MetaSystems/physics.h"
#include "Engine/Systems/systemManager.h"
#include "GLFW/glfw3.h"
#include "glm/ext/quaternion_geometric.hpp"
#include <glm/gtx/norm.hpp>

// Structs
namespace Star {

glm::vec2 PlayerInput::movement() {
    glm::vec2 movement = glm::vec2(right - left, up - down);
    if (glm::length2(movement)) movement = glm::normalize(movement);
    return movement;
}

float Player::movementSpeed() {
    return sizeFactor() * baseMoveSpeed;
}

float Player::accelerationSpeed() {
    return sizeFactor() * baseAccelerationSpeed;
}

float Player::sizeFactor() {
    return std::pow(growthFactor, starsCollected.size());
}

}

// INPUTS
namespace Star::Systems {
void playerInputButton(std::shared_ptr<Saga::GameWorld> world, int action,
                       const std::function<void (PlayerInput &, int)> &callback) {

    for (PlayerInput& playerInput : *world->viewAll<PlayerInput>())
        callback(playerInput, action);
}

void playerInputLeft(std::shared_ptr<Saga::GameWorld> world, int action) {
    playerInputButton(world, action, [](PlayerInput& input, int action) -> void {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            input.left = action == GLFW_PRESS;
    });
}

void playerInputRight(std::shared_ptr<Saga::GameWorld> world, int action) {
    playerInputButton(world, action, [](PlayerInput& input, int action) -> void {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            input.right = action == GLFW_PRESS;
    });
}

void playerInputUp(std::shared_ptr<Saga::GameWorld> world, int action) {
    playerInputButton(world, action, [](PlayerInput& input, int action) -> void {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            input.up = action == GLFW_PRESS;
    });
}

void playerInputDown(std::shared_ptr<Saga::GameWorld> world, int action) {
    playerInputButton(world, action, [](PlayerInput& input, int action) -> void {
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            input.down = action == GLFW_PRESS;
    });
}

void playerInputJump(std::shared_ptr<Saga::GameWorld> world, int action) {
    playerInputButton(world, action, [](PlayerInput& input, int action) -> void {
        if (action == GLFW_PRESS) {
            input.jump = PlayerInput::inputBufferTime;
            input.jumpRelease = 0;
        } else if (action == GLFW_RELEASE) {
            input.jumpRelease = PlayerInput::inputBufferTime;
        }
    });
}
}


namespace Star::Systems {

void playerController(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time) {
    auto mainCamera = world->viewAll<Saga::Camera>()->any();
    if (!mainCamera) return;

    auto group = *world->viewGroup<Star::Player, Star::PlayerInput, Saga::RigidBody, Saga::Transform>();

    // horizontal movement
    for (auto &[entity, player, playerInput, rigidBody, transform] : group) {
        glm::vec3 look = mainCamera.value()->camera->getLook();
        glm::vec2 movement = playerInput->movement();

        glm::vec3 moveX = glm::vec3(-look.z, 0, look.x);
        glm::vec3 moveY = glm::vec3(look.x, 0, look.z);

        glm::vec3 desiredVelocity = (moveX * movement.x + moveY * movement.y) * player->movementSpeed();

        glm::vec3 currentVelocity = rigidBody->velocity;
        currentVelocity.y = 0;
        glm::vec3 velocityDiff = desiredVelocity - currentVelocity;

        glm::vec3 accelerationDir = desiredVelocity - currentVelocity;
        if (!glm::length2(accelerationDir)) continue;

        accelerationDir = glm::normalize(accelerationDir);
        glm::vec3 frameAcceleration = accelerationDir * deltaTime * player->accelerationSpeed();

        if (glm::length2(frameAcceleration) > glm::length2(velocityDiff))
            frameAcceleration = velocityDiff;

        rigidBody->velocity += frameAcceleration;
        /* rigidBody->velocity.x = desiredVelocity.x; */
        /* rigidBody->velocity.z = desiredVelocity.z; */
    }

    auto group2 = *world->viewGroup<Saga::EllipsoidCollider, Star::Player, Star::PlayerInput,
         Saga::RigidBody, Saga::Transform>();

    // jump + gravity
    for (auto &[entity, ellipsoidCollider, player, playerInput, rigidBody, transform] : group2) {
        if (transform->getPos().y < player->minY)
            transform->transform->setPos(glm::vec3(0,player->maxY,10));

        float raycastDepth = 0.1f;
        float skinWidth = 0.001f;

        glm::vec3 groundCastPosition = transform->getPos() + glm::vec3(0,1,0) * skinWidth;
        glm::vec3 groundCastDir = glm::vec3(0,-1,0) * (skinWidth + raycastDepth);

        auto grounded = 
            Saga::Physics::ellipsoidCastAllTriangles(world,
                        groundCastPosition, groundCastDir, ellipsoidCollider->radius);

        rigidBody->velocity.y -= deltaTime * player->gravity;
        if (grounded) player->coyoteTime = playerInput->inputBufferTime;

        if (player->coyoteTime > 0 && playerInput->jump > 0) {
            rigidBody->velocity.y = player->jumpSpeed;
            player->coyoteTime = 0;
            playerInput->jump = 0;
        }

        if (grounded && rigidBody->velocity.y < 0) rigidBody->velocity.y = 0;

        player->coyoteTime -= deltaTime;
        playerInput->jump -= deltaTime;
        playerInput->jumpRelease -= deltaTime;
    }
}

void cameraControllerScroll(std::shared_ptr<Saga::GameWorld> world, double xpos, double ypos) {
    auto group = *world->viewGroup<Saga::Camera, Star::Camera, Star::PlayerInput, Saga::Transform>();
    for (auto &[entity, camera, cameraController, playerInput, transform] : group) {
        glm::vec2 mousePos(xpos, ypos);

        glm::vec2 mouseDelta = mousePos - cameraController->mousePosLastFrame;
        cameraController->mousePosLastFrame = mousePos;

        if (cameraController->isInitialFrame) {
            cameraController->isInitialFrame = false;
            continue;
        }

        glm::vec3 look = camera->camera->getLook();

        camera->camera->rotate(-mouseDelta.x * cameraController->turnRate.x, glm::vec3(0,1,0));
        camera->camera->rotate(mouseDelta.y * cameraController->turnRate.y, glm::vec3(look.z, 0, -look.x));

        glm::vec3 cachedCameraPos = camera->camera->getPos();

        if (glm::dot(glm::normalize(camera->camera->getLook()), glm::vec3(0,-1,0)) > 0.8) {
            // rotate back if looking down / up too far
            camera->camera->rotate(-mouseDelta.y * cameraController->turnRate.y, glm::vec3(look.z, 0, -look.x));
            continue;
        }

        glm::vec3 pos = transform->getPos() + cameraController->shoulderOffset
                        - camera->camera->getLook() * cameraController->distance;
        camera->camera->setPos(pos);
        if (camera->camera->getPos().y + 1 < transform->getPos().y) {
            camera->camera->rotate(-mouseDelta.y * cameraController->turnRate.y, glm::vec3(look.z, 0, -look.x));
            camera->camera->setPos(cachedCameraPos);
            continue;
        }
    }
}

void cameraControllerUpdate(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time) {
    auto group = *world->viewGroup<Saga::Camera, Star::Camera, Saga::Transform>();
    for (auto &[entity, camera, cameraController, transform] : group) {
        glm::vec3 pos = transform->getPos() + cameraController->shoulderOffset
                        - camera->camera->getLook() * cameraController->distance;
        camera->camera->setPos(pos);
    }
}

}

namespace Star::Systems {

void registerGroupsAndSystems(std::shared_ptr<Saga::GameWorld> world) {
    world->registerGroup<Star::Player, Star::PlayerInput, Saga::RigidBody, Saga::Transform>();
    world->registerGroup<Saga::EllipsoidCollider, Star::Player, Star::PlayerInput, Saga::RigidBody, Saga::Transform>();
    world->registerGroup<Saga::Camera, Star::Camera, Saga::Transform>();
    world->registerGroup<Saga::Camera, Star::Camera, Star::PlayerInput, Saga::Transform>();
    world->registerGroup<Comet, Saga::Transform>();

    auto& systems = world->getSystems();

    // keymap
    systems.addKeyboardEventSystem(GLFW_KEY_W, playerInputUp);
    systems.addKeyboardEventSystem(GLFW_KEY_S, playerInputDown);
    systems.addKeyboardEventSystem(GLFW_KEY_A, playerInputLeft);
    systems.addKeyboardEventSystem(GLFW_KEY_D, playerInputRight);
    systems.addKeyboardEventSystem(GLFW_KEY_SPACE, playerInputJump);

    // staged systems for camera
    systems.addStagedSystem(Saga::System<float,float>(cameraControllerUpdate), Saga::SystemManager::Stage::Update);
    systems.addMousePosSystem(Saga::System<double,double>(cameraControllerScroll));

    systems.addStagedSystem(Saga::System<float,float>(playerController), Saga::SystemManager::Stage::Update);
    systems.addStagedSystem(Saga::System<float,float>(animateStar), Saga::SystemManager::Stage::Update);

    systems.addStagedSystem(Saga::System<>(drawEditor), Saga::SystemManager::Stage::Draw);
}

}
