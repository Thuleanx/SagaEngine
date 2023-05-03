#include "collisionSystemOptimizationStatic.h"
#include <numeric>

namespace Saga::Systems {
    CollisionSystemData& getSystemData(std::shared_ptr<GameWorld> world) {
        auto allCollisionSystemData = world->viewAll<CollisionSystemData>();
        if (allCollisionSystemData->begin() == allCollisionSystemData->end()) {
            // if no collision data object exists we create one
            return *world->emplace<CollisionSystemData>(world->getMasterEntity());
        }
        return *allCollisionSystemData->begin();
    }

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

    std::optional<Collision> getClosestCollisionStatic(std::shared_ptr<GameWorld> world, 
        std::optional<CollisionSystemData*> systemData,
        Entity entity, EllipsoidCollider& ellipsoidCollider, glm::vec3 pos, glm::vec3 dir) {

        if (!systemData) systemData = &getSystemData(world);
        if (!systemData.value()->bvh) return {};

        auto hit = systemData.value()->bvh->traceEllipsoid(pos, dir, ellipsoidCollider.radius);

        if (hit) {
            auto [triangleData, t] = hit.value();
            glm::vec3 triangleNormal = glm::normalize(glm::cross(triangleData->triangle[1] - triangleData->triangle[0], triangleData->triangle[2] - triangleData->triangle[0]));
            return Collision {
                .t = t,
                .pos = t * dir + pos,
                .normal = triangleNormal,
                .entity0 = entity,
                .entity1 = triangleData->entity
            };
        }

        return {};
    }
}
