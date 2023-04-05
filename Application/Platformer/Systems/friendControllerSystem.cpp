#include "friendControllerSystem.h"
#include "Application/Platformer/Components/friendController.h"
#include "Engine/Components/rigidbody.h"
#include "Engine/Components/transform.h"
#include "Engine/Gameworld/gameworld.h"

namespace Platformer::Systems {
	void friendControllerSystem(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time) {
		for (auto &[entity, controller, rigidbody, transform] : 
            *world->viewGroup<FriendController, Saga::RigidBody, Saga::Transform>()) {

			// apply gravity
			rigidbody->velocity.y -= deltaTime * controller->gravity;

            // attract force inwards
            glm::vec3 d = transform->getPos();
            d.y = 0;

            // inward force scale squared of distance from the ideal orbit distance
            glm::vec3 inward = glm::vec3(0);
            /* glm::vec3 inward = -d * (glm::dot(d,d) - controller->orbitDistance * controller->orbitDistance); */
            // tangent speed is movement speed
            glm::vec3 tangent = glm::normalize(glm::vec3(-d.z, 0, d.x)) * controller->movementSpeed;

            // combined vertical, inward, and tangent speed
            rigidbody->velocity = rigidbody->velocity.y * glm::vec3(0,1,0) + inward + tangent;
        }
    }

	void registerFriendControllerSystem(std::shared_ptr<Saga::GameWorld> world) {
		world->registerGroup<
			FriendController,
            Saga::RigidBody,
            Saga::Transform
		>();
	}
}
