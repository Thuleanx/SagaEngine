#include "playerInputSystem.h"
#include "../Components/playerInput.h"
#include "Engine/saga.h"

namespace Application::Systems {
	void playerInputSystem_OnButton(std::shared_ptr<Saga::GameWorld> world, int action, 
        const std::function<void(PlayerInput&, int)>& callback) {

        for (PlayerInput& playerInput : *world->viewAll<PlayerInput>())
			callback(playerInput, action);
	}

	void playerInputSystem_OnLeftButton(std::shared_ptr<Saga::GameWorld> world, int action) {
		playerInputSystem_OnButton(world, action, [](PlayerInput& playerInput, int act) {
			if (act == GLFW_PRESS || act == GLFW_RELEASE)
				playerInput.left = act == GLFW_PRESS;
		});
	}

	void playerInputSystem_OnRightButton(std::shared_ptr<Saga::GameWorld> world, int action) {
		playerInputSystem_OnButton(world, action, [](PlayerInput& playerInput, int act) {
			if (act == GLFW_PRESS || act == GLFW_RELEASE)
				playerInput.right = act == GLFW_PRESS;
		});
	}

	void playerInputSystem_OnUpButton(std::shared_ptr<Saga::GameWorld> world, int action) {
		playerInputSystem_OnButton(world, action, [](PlayerInput& playerInput, int act) {
			if (act == GLFW_PRESS || act == GLFW_RELEASE)
				playerInput.up = act == GLFW_PRESS;
		});
	}

	void playerInputSystem_OnDownButton(std::shared_ptr<Saga::GameWorld> world, int action) {
		playerInputSystem_OnButton(world, action, [](PlayerInput& playerInput, int act) {
			if (act == GLFW_PRESS || act == GLFW_RELEASE)
				playerInput.down = act == GLFW_PRESS;
		});
	}

	void playerInputSystem_OnJumpButton(std::shared_ptr<Saga::GameWorld> world, int action) {
		playerInputSystem_OnButton(world, action, [](PlayerInput& playerInput, int act) {
			if (act == GLFW_PRESS) {
				playerInput.jumpTimer = playerInput.inputBufferTime;
				playerInput.jumpReleaseTimer = 0;
			} else if (act == GLFW_RELEASE) {
				playerInput.jumpReleaseTimer = playerInput.inputBufferTime;
			}
		});
	}
}
