#pragma once

namespace Platformer {
struct PlayerController {
	float movementSpeed;

	PlayerController() {}
	PlayerController(float movementSpeed) : movementSpeed(movementSpeed) {}

	float verticalVelocity = 0;
	float gravity = 2;
	float jumpVelocity = 15;

	float coyoteTimer;

	float coyoteTime = 0.1f;
};
} // namespace Platformer