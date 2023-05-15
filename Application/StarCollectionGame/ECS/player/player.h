#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <functional>

namespace Saga {
class GameWorld;
};

namespace Star {

struct PlayerInput {
    constexpr static float inputBufferTime = 0.1f;

    bool mouseDown;
    bool up = 0, down = 0, left = 0, right = 0;

    float jump;
    float jumpRelease;

    glm::vec2 movement();
};

struct Player {
    float movementSpeed = 3;
    float accelerationSpeed = 10;
    float gravity = 10;

    float coyoteTime;
    float jumpSpeed;
    int numStarsCollected = 0;
};

struct Camera {
    glm::vec3 focusOffset;
    glm::vec3 shoulderOffset;
    glm::vec2 turnRate;

    float distance;

    bool isInitialFrame = 1;
    glm::vec2 mousePosLastFrame;
};

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
void cameraController(std::shared_ptr<Saga::GameWorld> world, double xpos, double ypos);

void registerGroups(std::shared_ptr<Saga::GameWorld> world);

}
