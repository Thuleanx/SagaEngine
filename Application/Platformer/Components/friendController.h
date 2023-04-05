#pragma once

namespace Platformer {
struct FriendController {
	float movementSpeed;
    float orbitDistance;

	FriendController() {}
	FriendController(float movementSpeed, float orbitDistance) 
        : movementSpeed(movementSpeed), orbitDistance(orbitDistance) {}

	float verticalVelocity = 0;
	float gravity = 10;
	float jumpVelocity = 15;
};
} // namespace Platformer
