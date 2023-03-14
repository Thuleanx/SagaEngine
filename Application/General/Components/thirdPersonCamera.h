#pragma once
#include "Engine/saga.h"
#include <glm/vec3.hpp>

namespace Application {
struct ThirdPersonCamera {

	ThirdPersonCamera() {}
	ThirdPersonCamera(Saga::ComponentReference<Saga::Transform> playerTransform, 
		glm::vec3 shoulderOffset = glm::vec3(0,0.25f,0), glm::vec3 focusOffset = glm::vec3(0,1,1),
		glm::vec2 turnRate = glm::vec2(1.00f/50, 1.00f/50), float distance = 3) : 

		playerToFollow(playerTransform), shoulderOffset(shoulderOffset), focusOffset(focusOffset), turnRate(turnRate), distance(distance) {}

	Saga::ComponentReference<Saga::Transform> playerToFollow;
	glm::vec3 focusOffset;
	glm::vec3 shoulderOffset;
	glm::vec2 turnRate;
	float distance;

	bool isFirstFrame = 1;
	glm::vec3 playerPreviousPosition = glm::vec3(0,0,0);
	glm::vec2 mousePosLastFrame = glm::vec2(0,0);
};
} // namespace Warmup2
