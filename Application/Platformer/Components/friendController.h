#pragma once

namespace Platformer {
struct FriendController {
	float movementSpeed;

	FriendController() {}
	FriendController(float movementSpeed) : movementSpeed(movementSpeed) {}

	float verticalVelocity = 0;
	float gravity = 10;
	float jumpVelocity = 15;
};
} // namespace Platformer
