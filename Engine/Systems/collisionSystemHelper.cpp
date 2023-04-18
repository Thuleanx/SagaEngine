#include "collisionSystemHelper.h"
#include "Engine/Systems/collisionSystemOptimizationDynamic.h"
#include "collisionSystemOptimizationStatic.h"
#include "glm/gtx/string_cast.hpp"
#include "Engine/_Core/logger.h"

namespace Saga::Systems {

    std::optional<Collision> getClosestCollision(std::shared_ptr<GameWorld> world, 
        std::optional<CollisionSystemData *> systemData, Entity entity, 
        EllipsoidCollider &ellipsoidCollider, std::optional<CylinderCollider*> cylinderCollider, glm::vec3 pos, glm::vec3 dir) {

        std::optional<Collision> collision;

        auto possibleStaticCollision = getClosestCollisionStatic(world, 
                systemData, entity, ellipsoidCollider, pos, dir);

        if (possibleStaticCollision) collision = possibleStaticCollision.value();

        // detecting dynamic collision
        if (cylinderCollider) {
            auto possibleDynamicCollision = getClosestCollisionDynamic(world, systemData, entity, *cylinderCollider.value(), pos, dir);
            /* if (possibleDynamicCollision) */
                /* STRACE("Found collision at: %f, with position %s and normal %s.", */ 
                /*     possibleDynamicCollision->t.value(), */ 
                /*     glm::to_string(possibleDynamicCollision->pos.value()).c_str(), */  
                /*     glm::to_string(possibleDynamicCollision->normal.value()).c_str()); */

            if (possibleDynamicCollision && (!collision || collision->t > possibleDynamicCollision->t)) 
                collision = possibleDynamicCollision.value();
        }

        return collision;
    }

    glm::vec3 doNudge(std::shared_ptr<GameWorld> world, std::optional<CollisionSystemData *> systemData, Entity entity, EllipsoidCollider &ellipsoidCollider, std::optional<CylinderCollider *> cylinderCollider, glm::vec3 pos, Collision &collision) {
        const int MAX_NUDGES = 3;
        const float EPSILON = 0.0001f;
        const float nudgeAmt = 0.001f;

        glm::vec3 nudge = collision.normal;
        glm::vec3 pos_nudged = collision.pos + nudge * nudgeAmt;

        for (int i = 0; i < MAX_NUDGES; i++) {
            std::optional<Collision> nudge_collision = getClosestCollision(
                    world, systemData, entity, ellipsoidCollider, cylinderCollider,
                    pos, pos_nudged - pos);

            if (!nudge_collision) {
                pos = pos_nudged;
                break;
            } else {
                if (i == MAX_NUDGES-1) break; // might as well not do any computation if on last iteration

                glm::vec3 collisionNormal = nudge_collision->normal;
                glm::vec3 diff = collisionNormal - nudge;

                // this code is necessary when we hit an edge shared by two triangles in exactly the wrong way
                // this code tests whether we are nudged into the same triangle twice, and nudges you in the 
                // OPPOSITE direction if that happens
                /* if (glm::dot(diff, diff) < EPSILON) */ 
                /*     nudge = -collisionNormal; */
                /* else */
                /*     nudge = collisionNormal; */

                pos_nudged = nudge_collision->pos + nudge * nudgeAmt;

                // also adjust velocity so there wouldn't be any in the collision normal direction
                /* rigidBody.velocity -= glm::dot(rigidBody.velocity, nudge_collision.normal.value()) */ 
                /*     * nudge_collision.normal.value(); */
            }
        }
        return pos;
    }
}
