#pragma once

#include <glm/vec2.hpp>

namespace Application {
	struct PlayerInput {
		float inputBufferTime = 0.1;
		bool up = 0, down = 0, left = 0, right = 0;
		float jumpTimer = 0;
		float jumpReleaseTimer = 0;

		
		glm::vec2 getMovement() { return glm::vec2(right - left, up - down); }
	};
}
