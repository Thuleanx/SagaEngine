#pragma once

#include <memory>
#include "../Components/thirdPersonCamera.h"

namespace Saga { 
	class GameWorld; 
	struct Transform;
	struct Camera;
}

namespace Application::Systems {
	void thirdPersonCameraSystem(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time);
	void thirdPersonCameraSystem_OnMousePos(std::shared_ptr<Saga::GameWorld> world, double xpos, double ypos);
	void registerThirdPersonCameraSystem(std::shared_ptr<Saga::GameWorld> world);
}
