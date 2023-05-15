#include "physics.h"
#include "../Gameworld/gameworld.h"
#include "../Utils/geometry/geometry.h"
#include "../Components/_import.h"
#include "../_Core/asserts.h"
#include "Engine/Components/collider.h"
#include "Engine/Components/collisionSystemData.h"
#include "Engine/Components/transform.h"
#include "Engine/Systems/helpers/collisionSystemOptimizationStatic.h"

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

    std::optional<RaycastHit> raycastAllTriangles(std::shared_ptr<GameWorld> world, glm::vec3 pos, glm::vec3 dir) {
        CollisionSystemData& collisionSystemData = Saga::Systems::getSystemData(world);

        if (collisionSystemData.bvh) {
            auto hit = collisionSystemData.bvh->traceRay(pos, dir);
            if (!hit) return {};
            auto [triangleData, t] = hit.value();

            Collider* collider = world->getComponent<Collider>(triangleData->entity);

            return RaycastHit {
                .collider = collider,
                .t = t,
                .pos = pos + dir * t,
                .normal = glm::normalize(glm::cross(
                            triangleData->triangle[1]-triangleData->triangle[0],
                            triangleData->triangle[2]-triangleData->triangle[0]))
            };
        } 

        return {};
    }
}

}
