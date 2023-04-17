#pragma once
#include "collisionSystemOptimizationDynamic.h"

namespace Saga::Systems {
    inline void runOverGridCells(CollisionSystemData& collisionSystemData, glm::vec3 pos, glm::vec3 size, std::function<void(Entity)> callback) {
        if (collisionSystemData.uniformGrid) {
            for (int x = pos.x - size.x; x <= pos.x + size.x; x++) 
            for (int y = pos.y - size.y; y <= pos.y + size.y; y++) 
            for (int z = pos.z - size.z; z <= pos.z + size.z; z++) {
                auto cell = collisionSystemData.uniformGrid->getCell(x, y, z);
                if (cell) for (Entity entity : cell.value())
                    callback(entity);
            }
        }
    }

    inline void addToUniformGrid(CollisionSystemData& collisionSystemData, Entity entity, CylinderCollider& cylinderCollider, Transform& transform) {
        glm::vec3 pos = transform.getPos();
        for (int x = pos.x - cylinderCollider.radius; x <= pos.x + cylinderCollider.radius; x++) 
        for (int y = pos.y - cylinderCollider.height/2; y <= pos.y + cylinderCollider.height/2; y++) 
        for (int z = pos.z - cylinderCollider.radius; z <= pos.z + cylinderCollider.radius; z++) 
            collisionSystemData.uniformGrid.value().insert(x,y,z,entity);
    }

    inline void removeFromUniformGrid(CollisionSystemData& collisionSystemData, Entity entity, CylinderCollider& cylinderCollider, Transform& transform) {
        glm::vec3 pos = transform.getPos();
        for (int x = pos.x - cylinderCollider.radius; x <= pos.x + cylinderCollider.radius; x++) 
        for (int y = pos.y - cylinderCollider.height/2; y <= pos.y + cylinderCollider.height/2; y++) 
        for (int z = pos.z - cylinderCollider.radius; z <= pos.z + cylinderCollider.radius; z++) 
            collisionSystemData.uniformGrid.value().remove(x,y,z,entity);
    }
}
