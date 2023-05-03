#pragma once
#include "collisionSystemOptimizationDynamic.h"

namespace Saga::Systems {

    inline void runOverGridCells(CollisionSystemData& collisionSystemData, glm::vec3 pos, glm::vec3 size, std::function<void(Entity)> callback) {
        if (collisionSystemData.uniformGrid) {
            for (int x = (pos.x - size.x) / gridSize.x; x <= (pos.x + size.x) / gridSize.x; x++) 
            for (int y = (pos.y - size.y) / gridSize.y; y <= (pos.y + size.y) / gridSize.y; y++) 
            for (int z = (pos.z - size.z) / gridSize.z; z <= (pos.z + size.z) / gridSize.z; z++) {
                auto cell = collisionSystemData.uniformGrid->getCell(x, y, z);
                if (cell) for (Entity entity : cell.value())
                    callback(entity);
            }
        }
    }

    inline void addToUniformGrid(CollisionSystemData& collisionSystemData, Entity entity, CylinderCollider& cylinderCollider, Transform& transform) {
        glm::vec3 pos = transform.getPos();
        glm::vec3 size = glm::vec3(
            cylinderCollider.radius, cylinderCollider.height / 2, cylinderCollider.radius
        );
        for (int x = (pos.x - size.x) / gridSize.x; x <= (pos.x + size.x) / gridSize.x; x++)
        for (int y = (pos.y - size.y) / gridSize.y; y <= (pos.y + size.y) / gridSize.y; y++)
        for (int z = (pos.z - size.z) / gridSize.z; z <= (pos.z + size.z) / gridSize.z; z++)
            collisionSystemData.uniformGrid.value().insert(x,y,z,entity);
    }

    inline void removeFromUniformGrid(CollisionSystemData& collisionSystemData, Entity entity, CylinderCollider& cylinderCollider, Transform& transform) {
        glm::vec3 pos = transform.getPos();
        glm::vec3 size = glm::vec3(
            cylinderCollider.radius, cylinderCollider.height / 2, cylinderCollider.radius
        );
        for (int x = (pos.x - size.x) / gridSize.x; x <= (pos.x + size.x) / gridSize.x; x++)
        for (int y = (pos.y - size.y) / gridSize.y; y <= (pos.y + size.y) / gridSize.y; y++)
        for (int z = (pos.z - size.z) / gridSize.z; z <= (pos.z + size.z) / gridSize.z; z++)
            collisionSystemData.uniformGrid.value().remove(x,y,z,entity);
    }
}
