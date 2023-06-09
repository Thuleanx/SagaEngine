#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <functional>
#include <unordered_set>

namespace Saga {
class GameWorld;
};

namespace Star {

struct PlayerInput {
    constexpr static float inputBufferTime = 0.3f;

    bool mouseDown;
    bool up = 0, down = 0, left = 0, right = 0;

    float jump;
    float jumpRelease;

    glm::vec2 movement();
};

struct Player {
    float baseMoveSpeed;
    float baseAccelerationSpeed;
    float baseJumpSpeed;
    float gravity;
    float halfGravityThreshold;
    float coyoteTime;

    std::unordered_set<int> starsCollected;
    float minY, maxY;

    float growthFactor = 1.1;
    float growthValue = 1;


    // function of 
    float movementSpeed();
    float jumpSpeed();
    float accelerationSpeed();
    glm::vec3 glowValue();
};

struct Camera {
    glm::vec3 focusOffset;
    glm::vec3 shoulderOffset;
    glm::vec2 turnRate;

    float realDistance;
    float distance;
    float distanceSmoothing;
    float positionalSmoothing;

    bool isInitialFrame = 1;
    glm::vec2 mousePosLastFrame;
};

namespace Systems {

// INPUTS
void playerInputButton(std::shared_ptr<Saga::GameWorld> world, int action,
                       const std::function<void(PlayerInput&, int)>& callback);
void playerInputMouse(std::shared_ptr<Saga::GameWorld> world, int action);

void playerInputLeft(std::shared_ptr<Saga::GameWorld> world, int action);
void playerInputRight(std::shared_ptr<Saga::GameWorld> world, int action);
void playerInputUp(std::shared_ptr<Saga::GameWorld> world, int action);
void playerInputDown(std::shared_ptr<Saga::GameWorld> world, int action);

void playerInputJump(std::shared_ptr<Saga::GameWorld> world, int action);

// MOVEMENTS
void playerController(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time);

// CAMERA
void cameraControllerUpdate(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time);
void cameraControllerScroll(std::shared_ptr<Saga::GameWorld> world, double xpos, double ypos);

void registerGroupsAndSystems(std::shared_ptr<Saga::GameWorld> world);
}

}
