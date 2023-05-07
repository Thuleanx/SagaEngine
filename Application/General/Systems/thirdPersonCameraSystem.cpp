#include "thirdPersonCameraSystem.h"
#include "../Components/thirdPersonCamera.h"
#include "Application/General/Components/playerInput.h"
#include "Engine/saga.h"

namespace Application::Systems {
	namespace {
		void recomputeCameraPosition(Saga::Transform* playerTransform, Saga::Camera* camera, 
			ThirdPersonCamera* tpcamera, Saga::Transform* transform) {
			glm::vec3 pos = playerTransform->transform->getPos() + tpcamera->shoulderOffset - camera->camera->getLook() * tpcamera->distance;
			camera->camera->setPos(pos);
		}
	}

	void thirdPersonCameraSystem(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time) {
		for (auto &[entity, camera, tpcamera, transform] : 
			*world->viewGroup<Saga::Camera, ThirdPersonCamera, Saga::Transform>()) {

			if (tpcamera->playerToFollow) {
				Saga::Transform &playerTransform = *tpcamera->playerToFollow;

				glm::vec3 currentPlayerPos = playerTransform.transform->getPos();

				if (currentPlayerPos != transform->transform->getPos())
					transform->transform->setPos(currentPlayerPos);

				recomputeCameraPosition(&playerTransform, camera, tpcamera, transform);
				tpcamera->playerPreviousPosition = currentPlayerPos;
			}
		}
	}

	void registerThirdPersonCameraSystem(std::shared_ptr<Saga::GameWorld> world) {
		world->registerGroup<Saga::Camera, ThirdPersonCamera, Saga::Transform>();
		world->registerGroup<Saga::Camera, PlayerInput, ThirdPersonCamera, Saga::Transform>();
	}

	void thirdPersonCameraSystem_OnMousePos(std::shared_ptr<Saga::GameWorld> world, double xpos, double ypos) {
		for (auto &[entity, camera, tpcamera, transform] : 
			*world->viewGroup<Saga::Camera, ThirdPersonCamera, Saga::Transform>()) {

			glm::vec2 mousePos(xpos, ypos);
            std::optional<Saga::Entity> player = tpcamera->playerToFollow.getEntity();
            if (!player) continue;
            auto playerInput = world->getComponent<PlayerInput>(player.value());

			if (!tpcamera->isFirstFrame && playerInput && playerInput->mouseDown) {
				glm::vec2 mouseDelta = mousePos - tpcamera->mousePosLastFrame;

				glm::vec3 look = camera->camera->getLook();

				camera->camera->rotate(-mouseDelta.x * tpcamera->turnRate.x, glm::vec3(0,1,0));

				glm::vec3 cachedCameraPos = camera->camera->getPos();
				camera->camera->rotate(mouseDelta.y * tpcamera->turnRate.y, glm::vec3(look.z, 0, -look.x));

				if (glm::dot(glm::normalize(camera->camera->getLook()), glm::vec3(0,-1,0)) > 0.8) {
					camera->camera->rotate(-mouseDelta.y * tpcamera->turnRate.y, glm::vec3(look.z, 0, -look.x));
				} else if (tpcamera->playerToFollow) {
                    recomputeCameraPosition(tpcamera->playerToFollow, camera, tpcamera, transform);
					if (camera->camera->getPos().y < tpcamera->playerToFollow->transform->getPos().y) {
						camera->camera->rotate(-mouseDelta.y * tpcamera->turnRate.y, glm::vec3(look.z, 0, -look.x));
						camera->camera->setPos(cachedCameraPos);
					}
				}
			} else tpcamera->isFirstFrame = false;

			tpcamera->mousePosLastFrame = mousePos;
		}
	}
}
