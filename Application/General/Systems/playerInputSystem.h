#pragma once

#include <memory>
#include <functional>
#include "../Components/playerInput.h"

namespace Saga { class GameWorld; }

namespace Application::Systems {
	void playerInputSystem_OnButton(std::shared_ptr<Saga::GameWorld> world, int action, 
		const std::function<void(PlayerInput&, int)>& callback);
	void playerInputSystem_OnMouseButton(std::shared_ptr<Saga::GameWorld> world, int action);
	void playerInputSystem_OnLeftButton(std::shared_ptr<Saga::GameWorld> world, int action);
	void playerInputSystem_OnLeftButton(std::shared_ptr<Saga::GameWorld> world, int action);
	void playerInputSystem_OnRightButton(std::shared_ptr<Saga::GameWorld> world, int action);
	void playerInputSystem_OnUpButton(std::shared_ptr<Saga::GameWorld> world, int action);
	void playerInputSystem_OnDownButton(std::shared_ptr<Saga::GameWorld> world, int action);
	void playerInputSystem_OnJumpButton(std::shared_ptr<Saga::GameWorld> world, int action);
}
