#include "collisionSystem.h"
#include "../Gameworld/gameworld.h"
#include "../Components/_import.h"
#include <glm/vec3.hpp>
#include "Engine/Components/collisionSystemData.h"
#include "Engine/Datastructures/Accelerant/boundingBox.h"
#include "events.h"
#include "Graphics/modeltransform.h"
#include "../Utils/geometry.h"
#include "../_Core/asserts.h"
#include "glm/gtx/string_cast.hpp"

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

            // only resolve 10 collisions per frame at most
            while (collisionResolvingCount > 0) {
                bool collisionDetected = false;
                auto &group = *world->viewGroup<Saga::Collider,Saga::CylinderCollider, Saga::RigidBody, Saga::Transform>();
                for (auto &[entity0, collider0, cylinderCollider0, rigidbody0, transform0] : group) 
                    for (auto &[entity1, collider1, cylinderCollider1, rigidbody1, transform1] : group) {

                        // null checks
                        SASSERT_MESSAGE(transform0->transform, "Transform cannot be null.");
                        SASSERT_MESSAGE(transform1->transform, "Transform cannot be null.");

                        // only detect collision if one of the objects is not static
                        if (entity0 < entity1 && (!rigidbody0->isStatic() || !rigidbody1->isStatic())) {
                            // detect collision between the two cylinders
                            glm::vec3 mtv = detectCollision( *cylinderCollider0, *transform0, *cylinderCollider1, *transform1 );

                            if (mtv != glm::vec3(0,0,0)) {
                                // deliver the collision events to the two entities
                                world->deliverEvent(EngineEvents::OnCollision, entity0, entity1);
                                world->deliverEvent(EngineEvents::OnCollision, entity1, entity0);

                                collisionDetected = 1;

                                float eps = 0.00001f;
                                // default to moving both by 0.5
                                float move0 = 0.5f + eps, move1 = 0.5f + eps;

                                // if one is static, the other move the full way
                                if (rigidbody0->isStatic()) move1 = 1 + eps, move0 = 0;
                                if (rigidbody1->isStatic()) move0 = 1 + eps, move1 = 0;

                                transform0->transform->translate(mtv * move0);
                                transform1->transform->translate(-mtv * move1);

                                if (collisionResolvingCount-- <= 0) 
                                    goto endCollisions;

                            }
                        }
                    }


                if (!collisionDetected) break;
            }

endCollisions: {}
        }

        /**
         * @brief Retrieve the world's CollisionSystemData. This should live as
         * a component on an entity. If none exists, an entity with the component
         * will be created.
         *
         * @param world the world that the data exists on. 
         *
         * @return a reference to the CollisionSystemData that is in this world.
         */
        CollisionSystemData& getSystemData(std::shared_ptr<GameWorld> world) {
            auto allCollisionSystemData = world->viewAll<CollisionSystemData>();
            if (allCollisionSystemData->begin() == allCollisionSystemData->end()) {
                // if no collision data object exists
                // we create one
                Entity containerEntity = world->createEntity();
                return *world->emplace<CollisionSystemData>(containerEntity);
            }
            return *allCollisionSystemData->begin();
        }

        /**
         * @brief This builds the bounding volume hierarchy for all static meshes
         * in the scene, and stores it inside of CollisionSystemData.
         */
        void rebuildStaticBVH(std::shared_ptr<GameWorld> world) {
            CollisionSystemData& collisionSystemData = getSystemData(world);
            if (!collisionSystemData.bvh.has_value())
                collisionSystemData.bvh = BoundingVolumeHierarchy();

            std::vector<BoundingVolumeHierarchy::TriangleData> allTriangles;

            // first aggregate all triangle data
            for (auto &[entity, collider, mesh, meshCollider, transform] : *world->viewGroup<Collider, Mesh, MeshCollider, Transform>()) {
                // grab all triangles in the mesh
                for (int triangleIndex = 0; triangleIndex < mesh->getTrianglesCnt(); triangleIndex++) {
                    BoundingVolumeHierarchy::TriangleData triangleData;
                    triangleData.entity = entity;

                    // transform all triangles to world space
                    for (int j = 0; j < 3; j++)
                        triangleData.triangle[j] = transform->transform->getModelMatrix() * glm::vec4(mesh->getPos(3*triangleIndex + j), 1);

                    allTriangles.push_back(triangleData);
                }
            }

            collisionSystemData.bvh.value().build(allTriangles);
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
                Entity entityEllipsoid, Transform& transform, EllipsoidCollider& ellipsoidCollider, RigidBody& rigidBody, float deltaTime) {

            glm::vec3 curPos = transform.transform->getPos();
            glm::vec3 nextPos = curPos + rigidBody.velocity * deltaTime;

            const int MAX_TRANSLATIONS = 100;
            const float nudgeAmt = 0.0001f;

            std::vector<Collision> collisions;

            auto sysData = getSystemData(world);
            auto allCylinders = *world->viewGroup<Collider, CylinderCollider, Transform>();

            CylinderCollider* cylinderCollider = world->getComponent<CylinderCollider>(entityEllipsoid);

            for (int i = 0; i < MAX_TRANSLATIONS; i++) {
                glm::vec3 dir = nextPos - curPos;

                // get closest collision
                Collision collision;

                // detecting static collision
                if (sysData.bvh) {
                    std::optional<std::tuple<BoundingVolumeHierarchy::TriangleData*, float>> hit = sysData.bvh.value().traceEllipsoid(curPos, dir, ellipsoidCollider.scale);

                    if (hit) {
                        BoundingVolumeHierarchy::TriangleData* data = std::get<0>(hit.value());
                        float tc = std::get<1>(hit.value());
                        glm::vec3 triangleNormal = glm::normalize(glm::cross(data->triangle[1] - data->triangle[0], data->triangle[2] - data->triangle[0]));
                        collision = Collision(tc, tc * dir + curPos, triangleNormal, entityEllipsoid, data->entity);
                    }
                }

                // detecting dynamic collision
                if (cylinderCollider) {
                    for (auto &[otherEntity, otherCollider, otherCylinderCollider, otherTransform] 
                            : allCylinders) if (otherEntity != entityEllipsoid) {

                        std::optional<std::tuple<float, glm::vec3>> hit = Saga::Geometry::movingCylinderCylinderIntersection(cylinderCollider->height, cylinderCollider->radius, 
                            transform.getPos(), otherCylinderCollider->height, otherCylinderCollider->radius, otherTransform->getPos(), dir);

                        if (hit) {
                            float tc = std::get<0>(hit.value());
                            if (!collision.t || collision.t.value() > tc) 
                                collision = Collision(tc, tc * dir + curPos, std::get<1>(hit.value()), entityEllipsoid, otherEntity);
                        }
                    }
                }


                if (!collision.t) {
                    return make_pair(nextPos, collisions);
                } else {
                    /* STRACE("Found collision at: %f, with position %s and normal %s.", collision.t, glm::to_string(collision.pos).c_str(),  glm::to_string(collision.normal).c_str()); */

                    // nudge the position a bit long the collision normal
                    curPos = collision.pos.value() + collision.normal.value() * nudgeAmt;

                    dir = nextPos - curPos;
                    // correct direction by negating it in the normal direction to the collision.
                    glm::vec3 dirCorrected = dir - glm::dot(dir, collision.normal.value()) * collision.normal.value();
                    nextPos = curPos + dirCorrected;

                    // also adjust velocity so there wouldn't be any in the collision normal direction
                    rigidBody.velocity -= glm::dot(rigidBody.velocity, collision.normal.value()) * collision.normal.value();

                    collisions.push_back(collision);
                }
            }

            return make_pair(curPos, collisions);
        }
    }

    /**
     * @brief First handle cylinder-cylinder collision. Then handle all triangle-ellipsoid collisions.
     */
    void collisionSystem(std::shared_ptr<GameWorld> world, float deltaTime, float time) {
        cylinderCylinderCollision(world, deltaTime, time);
        for (auto &[entity, collider, ellipsoidCollider, rigidBody, transform] : *world->viewGroup<Collider, EllipsoidCollider, RigidBody, Transform>()) {
            std::pair<glm::vec3, std::vector<Collision>> res = ellipsoidTriangleCollisions(world, entity, *transform, *ellipsoidCollider, *rigidBody, deltaTime);
            transform->transform->setPos(res.first);
        }
    }

    /**
     * @brief Builds the bounding volume hierarchy of the scene's static triangles. Used for ellipsoid-triangle collisions
     */
    void collisionSystem_startup(std::shared_ptr<GameWorld> world) {
        rebuildStaticBVH(world);
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
