#include "physics.h"
#include "../Gameworld/gameworld.h"
#include "../Utils/geometry/geometry.h"
#include "../Components/_import.h"
#include "../_Core/asserts.h"

namespace Saga {

namespace Physics {
	bool overlapCylinder(std::shared_ptr<GameWorld> world, float height, float radius, glm::vec3 pos) {
		for (auto &[entity, collider, cylinderCollider, rigidbody, transform] : 
			*world->viewGroup<Saga::Collider,Saga::CylinderCollider, Saga::RigidBody, Saga::Transform>()) {
			
			SASSERT_MESSAGE(transform->transform, "Transform cannot be null.");
			
            glm::vec3 mtv = Saga::Geometry::detectAACylinderCylinderCollision(
				height, radius, pos, cylinderCollider->height, cylinderCollider->radius, transform->transform->getPos()
			);

			if (mtv != glm::vec3(0,0,0))
				return true;
		}
		return false;
	}

	void registerPhysicsMetaSystem(std::shared_ptr<GameWorld> world) {
		world->registerGroup<Collider, CylinderCollider, RigidBody, Transform>();
	}
}

}
