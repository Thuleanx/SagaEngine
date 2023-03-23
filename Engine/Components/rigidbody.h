#pragma once

#include <glm/vec3.hpp>

namespace Saga {

/**
 * @brief Entities with a RigidBody will be moved by the engine in FixedUpdate,
 * 	and will respond to collisions.
 * @ingroup component
 */
struct RigidBody {
	/**
	 * @brief Static Rigidbody do not move, and is unaffected by the physics engine.
	 * Dynamic RigidBody move and collide with Static or other Dynamic RigidBodies.
	 */
	enum Mode {
		Dynamic,
		Static
	};

	RigidBody(Mode mode = RigidBody::Mode::Dynamic) : mode(mode) {}

	Mode mode;                               //!< Mode of this rigidbody. Either Dynamic or Static.
	glm::vec3 velocity = glm::vec3(0, 0, 0); //!< Velocity of this rigidbody. Controlled by the Application as well as the Physics Engine.

	/**
	 * @return true if the rigidBody is Static
	 * @return false otherwise
	 */
	bool isStatic() { return mode == Static; }
};

} // namespace Saga