#include "playerControllerSystem.h"
#include "Application/General/Components/playerInput.h"
#include "../Components/playerController.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "Engine/saga.h"
#include "glm/gtx/string_cast.hpp"

namespace Platformer::Systems {

	// anonymous namespace
	namespace {

	}

	void playerControllerSystem(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time) {
		Saga::Camera& mainCamera = *world->viewAll<Saga::Camera>()->begin();
		for (auto &[entity, controller, input, rigidbody, transform] : *world->viewGroup<PlayerController, Application::PlayerInput, Saga::RigidBody, Saga::Transform>()) {
			glm::vec3 moveAmount(0,0,0);

			glm::vec3 look = mainCamera.camera->getLook();
			glm::vec2 movement = input->getMovement();
			if (glm::dot(movement, movement) > 0) movement = glm::normalize(movement);

			glm::vec3 moveX = glm::vec3(-look.z, 0, look.x);
			glm::vec3 moveY = glm::vec3(look.x, 0, look.z);
			glm::vec3 horizontalVelocity = (moveX * movement.x + moveY * movement.y) * controller->movementSpeed;

			// horizontal speed
			rigidbody->velocity.x = horizontalVelocity.x;
			rigidbody->velocity.z = horizontalVelocity.z;

			glm::vec3 groundPos = transform->transform->getScale() * 0.5f * glm::vec3(0,-1,0) + transform->transform->getPos();

			float depthOfGroundCast = 0.01f;
			float skinWidth = 0.01f;
			float eps = 0.001f;

			bool grounded = Saga::Physics::overlapCylinder(world, depthOfGroundCast, 0.5f - eps,  
				groundPos + glm::vec3(0, -1, 0) * (depthOfGroundCast / 2 + skinWidth)
			);

			if (grounded) controller->coyoteTimer = controller->coyoteTime;

			// apply gravity
			rigidbody->velocity.y -= deltaTime * controller->gravity;

			// see if want to jump
			if (controller->coyoteTimer && input->jumpTimer) {
				rigidbody->velocity.y = controller->jumpVelocity;
				controller->coyoteTimer = 0;
				input->jumpTimer = 0;
			}

			if (grounded && controller->verticalVelocity < 0) controller->verticalVelocity = 0;

			if (controller->verticalVelocity)
				moveAmount += controller->verticalVelocity * glm::vec3(0,1,0) * deltaTime;

			controller->coyoteTimer = std::max(controller->coyoteTimer - deltaTime, 0.0f);
			input->jumpTimer = std::max(input->jumpTimer - deltaTime, 0.0f);
			input->jumpReleaseTimer = std::max(input->jumpReleaseTimer - deltaTime, 0.0f);
		}
	}

	void registerPlayerControllerSystem(std::shared_ptr<Saga::GameWorld> world) {
		world->registerGroup<
			PlayerController,
			Application::PlayerInput,
			Saga::RigidBody,
			Saga::Transform
		>();
	}
}
