#include "collisionSystem.h"
#include "Engine/Components/collider.h"
#include "collisionSystemOptimizationStatic.h"
#include "collisionSystemOptimizationDynamic.h"
#include "collisionSystemHelper.h"

#include "../Gameworld/gameworld.h"
#include "../Components/_import.h"
#include "../Utils/geometry/geometry.h"
#include "../_Core/asserts.h"
#include <glm/vec3.hpp>
#include "Engine/Components/collisionSystemData.h"
#include "events.h"
#include "Graphics/modeltransform.h"
#include <unordered_set>
#include <functional>

namespace Saga::Systems {
    // anonymous namespace
    namespace {
        /**
         * @brief Detect collision between two axis-aligned cylinder colliders, and return the mtv such that if the first cylinder is translated by the mtv, the two objects no longer collide.
         * 
         * @param a the first cylinder collider.
         * @param aTransform the first cylinder transform, used to position the cylinder in space.
         * @param b the second cylinder collider.
         * @param bTransform the second cylinder transform, used to position the cylinder in space.
         * @return glm::vec3 zero if the two cylinders are not colliding. Otherwise the mtv.
         */
        glm::vec3 detectCollision(
                CylinderCollider &a, Transform &aTransform, 
                CylinderCollider &b, Transform &bTransform) {

            return Saga::Geometry::detectAACylinderCylinderCollision(a.height, a.radius, aTransform.transform->getPos(),
                    b.height, b.radius, bTransform.transform->getPos());
        }

        /**
         * @brief Handle collisions between a moving ellipsoid and triangles in the scene.
         * By default, this does 100 translations before dropping further movement.
         * 
         * @param world 
         * @param entityEllipsoid entity the ellipsoid is attached to.
         * @param transform the entity's transform.
         * @param ellipsoidCollider the entity's ellipsoid collider.
         * @param rigidBody the entity's rigid body.
         * @param deltaTime time since last update call, in seconds.
         * @return std::pair<glm::vec3, std::vector<Collision>> pair containing the ending position of the ellipsoid, as well as a list of all collisions that the ellipsoid triggered while moving.
         */
        std::pair<glm::vec3, std::vector<Collision>> ellipsoidTriangleCollisions(std::shared_ptr<GameWorld> world, 
                Entity entityEllipsoid, Transform& transform, EllipsoidCollider& ellipsoidCollider, 
                RigidBody& rigidBody, glm::vec3 move) {

            glm::vec3 curPos = transform.transform->getPos();
            glm::vec3 nextPos = curPos + move;

            const int MAX_TRANSLATIONS = 10;

            std::vector<Collision> collisions;

            std::optional<CylinderCollider*> cylinder;

            {
                CylinderCollider* cylinderCollider = world->getComponent<CylinderCollider>(entityEllipsoid);
                if (cylinderCollider) cylinder = cylinderCollider;
            }

            auto sysData = getSystemData(world);

            for (int i = 0; i < MAX_TRANSLATIONS; i++) {
                glm::vec3 dir = nextPos - curPos;

                // get closest collision
                std::optional<Collision> collision = 
                    getClosestCollision(world, &sysData, entityEllipsoid, ellipsoidCollider, cylinder, curPos, dir);

                if (!collision) {
                    return make_pair(nextPos, collisions);
                } else {
                    /* STRACE("Found collision at: %f, with position %s and normal %s.", collision.t.value(), glm::to_string(collision.pos.value()).c_str(),  glm::to_string(collision.normal.value()).c_str()); */

                    // nudge the position a bit long the collision normal
                    curPos = collision->pos.value() + 0.001f * collision->normal.value();
                    /* curPos = doNudge(world, &sysData, entityEllipsoid, ellipsoidCollider, */
                    /*     cylinderCollider == nullptr ? nullptr : cylinderCollider, */
                    /*     curPos, collision.value()); */

                    dir = nextPos - curPos;
                    // correct direction by negating it in the normal direction to the collision.
                    glm::vec3 dirCorrected = dir - glm::dot(dir, collision->normal.value()) * collision->normal.value();
                    nextPos = curPos + dirCorrected;

                    // also adjust velocity so there wouldn't be any in the collision normal direction
                    rigidBody.velocity -= glm::dot(rigidBody.velocity, collision->normal.value()) * collision->normal.value();

                    collisions.push_back(collision.value());
                }
            }

            return make_pair(curPos, collisions);
        }

        /**
         * @brief Process all cylinder-cylinder collisions. 
         * If two cylinders penetrate, they move by half of the minimum translation vector away from each other if they are both dynamic.
         * Otherwise, only the dynamic one move and moves by the full mtv, plus some epsilon.
         * By default, this only resolve 100 collisions per call.
         * 
         * @param world 
         * @param deltaTime 
         * @param time time ellapsed since the start of the program.
         */
        void cylinderCylinderCollision(std::shared_ptr<GameWorld> world, float deltaTime, float time) {
            int collisionResolvingCount = 10;

            auto handleCollision = 
            []( std::shared_ptr<GameWorld> world, glm::vec3 mtv,
                auto entity0, auto collider0, auto cylinderCollider0, auto rigidbody0, auto transform0,
                auto entity1, auto collider1, auto cylinderCollider1, auto rigidbody1, auto transform1) {

                // deliver the collision events to the two entities
                world->deliverEvent(EngineEvents::OnCollision, entity0, entity1);
                world->deliverEvent(EngineEvents::OnCollision, entity1, entity0);

                float eps = 0.001f;
                // default to moving both by 0.5
                float move0 = 0.5f + eps, move1 = 0.5f + eps;

                // if one is static, the other move the full way
                if (rigidbody0->isStatic()) move1 = 1 + eps, move0 = 0;
                if (rigidbody1->isStatic()) move0 = 1 + eps, move1 = 0;

                EllipsoidCollider* ellipsoid0 = world->getComponent<EllipsoidCollider>(entity0);
                EllipsoidCollider* ellipsoid1 = world->getComponent<EllipsoidCollider>(entity1);

                if (ellipsoid0) { 
                    ellipsoidTriangleCollisions(world, entity0, *transform0, *ellipsoid0, *rigidbody0, mtv*move0);
                } else transform0->transform->translate(mtv * move0);

                if (ellipsoid1) {
                    ellipsoidTriangleCollisions(world, entity1, *transform1, *ellipsoid1, *rigidbody1, mtv*move1);
                } else transform1->transform->translate(-mtv * move1);

            };

            std::set<std::pair<Entity, Entity>> collisionPair;

            // only resolve 10 collisions per frame at most
            while (collisionResolvingCount > 0) {
                bool collisionDetected = false;
                auto &group = *world->viewGroup<Saga::Collider,Saga::CylinderCollider, Saga::RigidBody, Saga::Transform>();
                for (auto &[entity0, collider0, cylinderCollider0, rigidbody0, transform0] : group) 
                    for (auto &[entity1, collider1, cylinderCollider1, rigidbody1, transform1] : group) 

                    if (!collisionPair.count(std::make_pair(entity0, entity1))) {

                        // null checks
                        SASSERT_MESSAGE(transform0->transform, "Transform cannot be null.");
                        SASSERT_MESSAGE(transform1->transform, "Transform cannot be null.");

                        // only detect collision if one of the objects is not static
                        if (entity0 < entity1 && (!rigidbody0->isStatic() || !rigidbody1->isStatic())) {
                            // detect collision between the two cylinders
                            glm::vec3 mtv = detectCollision( *cylinderCollider0, *transform0, *cylinderCollider1, *transform1 );

                            if (mtv != glm::vec3(0,0,0)) {
                                collisionDetected = 1;
                                handleCollision(world, mtv, 
                                        entity0,collider0,  cylinderCollider0,  rigidbody0,  transform0,  
                                        entity1,  collider1,  cylinderCollider1,  rigidbody1,  transform1);
                                if (collisionResolvingCount-- <= 0)
                                    goto endCollisions;
                                collisionPair.insert(std::make_pair(entity0, entity1));
                            }
                        }
                    }
                if (!collisionDetected) break;
            }

endCollisions: {}
        }

    }

    /**
     * @brief First handle cylinder-cylinder collision. Then handle all triangle-ellipsoid collisions.
     */
    void collisionSystem(std::shared_ptr<GameWorld> world, float deltaTime, float time) {
        for (auto &[entity, collider, ellipsoidCollider, rigidBody, transform] : *world->viewGroup<Collider, EllipsoidCollider, RigidBody, Transform>()) {
            Saga::CylinderCollider* cylinder = world->getComponent<CylinderCollider>(entity);
            if (cylinder) removeFromUniformGrid(getSystemData(world), entity, *cylinder, *transform);

            std::pair<glm::vec3, std::vector<Collision>> res = ellipsoidTriangleCollisions(world, entity, *transform, 
                *ellipsoidCollider, *rigidBody, deltaTime * rigidBody->velocity);
            transform->transform->setPos(res.first);

            if (cylinder) addToUniformGrid(getSystemData(world), entity, *cylinder, *transform);
        }
        /* cylinderCylinderCollision(world, deltaTime, time); */
    }

    /**
     * @brief Builds the bounding volume hierarchy of the scene's static triangles. Used for ellipsoid-triangle collisions
     */
    void collisionSystem_startup(std::shared_ptr<GameWorld> world) {
        rebuildStaticBVH(world);
        rebuildUniformGrid(world);
    }

    void registerCollisionSystem(std::shared_ptr<GameWorld> world) {
        world->registerGroup<Saga::Collider, Saga::CylinderCollider, Saga::RigidBody, Saga::Transform>();
        world->registerGroup<Saga::Collider, Saga::CylinderCollider, Saga::Transform>();
        world->registerGroup<Saga::Collider, Saga::Mesh, Saga::MeshCollider, Saga::Transform>();
        world->registerGroup<Saga::Collider, Saga::EllipsoidCollider, Saga::RigidBody, Saga::Transform>();

		auto& systems = world->getSystems();
		// collision handling on fixedUpdate
		systems.addStagedSystem(std::make_shared<Saga::System<>>(collisionSystem_startup), Saga::SystemManager::Stage::Awake);
		systems.addStagedSystem(std::make_shared<Saga::System<float, float>>(collisionSystem), Saga::SystemManager::Stage::FixedUpdate);
    }
}
