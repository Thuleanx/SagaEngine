#include "collisionSystemOptimizationDynamic.h"
#include "Engine/Components/collider.h"
#include "Engine/Components/transform.h"
#include "collisionSystemOptimizationStatic.h"
#include "Engine/Utils/geometry/cylinder.h"
#include <unordered_set>

namespace Saga::Systems {

    void rebuildUniformGrid(std::shared_ptr<GameWorld> world) {
        CollisionSystemData& collisionSystemData = getSystemData(world);
        if (!collisionSystemData.uniformGrid)
            collisionSystemData.uniformGrid = UniformGrid<Entity>();

        auto allCylinders = *world->viewGroup<Collider, CylinderCollider, Transform>();
        for (auto [entity, collider, cylinderCollider, transform] : allCylinders) 
            addToUniformGrid(collisionSystemData, entity, *cylinderCollider, *transform);
    }

    std::optional<Collision> getClosestCollisionDynamic(std::shared_ptr<GameWorld> world, 
        std::optional<CollisionSystemData*> systemData, 
        Entity entity, CylinderCollider &cylinderCollider, glm::vec3 pos, glm::vec3 dir) {

        std::unordered_set<Entity> visited;
        // if systemData was not passed in, we grab it here.
        if (!systemData) systemData = &getSystemData(world);

        // twice the size of the bounding box of the cylinder. Here we assume the cylinder does not move
        // that much over 1 frame. More correct thing to do would be to 
        // search over the bounding box of the movement.
        glm::vec3 sizeToSearch = glm::vec3(cylinderCollider.radius, cylinderCollider.height/2, cylinderCollider.radius);

        std::optional<Collision> collision;

        for (auto [otherEntity, collider, otherCylinderCollider, otherTransform] : *world->viewGroup<Collider, CylinderCollider, Transform>()) {

        /* runOverGridCells(*systemData.value(), pos + dir, sizeToSearch, [&](Entity otherEntity) { */
            // we dont allow self-collision
            if (otherEntity == entity) continue; 

            /* if (visited.count(otherEntity)) continue; */
            /* visited.insert(otherEntity); */

            /* auto otherCylinderCollider = world->getComponent<CylinderCollider>(otherEntity); */
            /* auto otherTransform = world->getComponent<Transform>(otherEntity); */

            // if somehow the other object got into the uniform grid in a weird way
            /* if (!otherCylinderCollider || !otherTransform) continue; */

            // perform collision detection
            auto hit = Saga::Geometry::movingCylinderCylinderIntersection(
                cylinderCollider.height, cylinderCollider.radius, pos, 
                otherCylinderCollider->height, otherCylinderCollider->radius, otherTransform->getPos(), dir);

            if (!hit) continue;

            auto [tc, normal] = hit.value();

            // if we already found a better collision, ignore
            if (collision && collision->t <= tc) continue;

            collision = Collision {
                .t = tc, 
                .pos = tc * dir + pos, 
                .normal = normal, 
                .entity0 = entity, 
                .entity1 = otherEntity
            };
        /* }); */
        }

        return collision;
    }
}
