#pragma once

#include "Engine/Components/collider.h"
#include "Engine/Components/collisionSystemData.h"
#include "Engine/Components/transform.h"
#include "Engine/Entity/entity.h"
#include "Engine/Systems/collisionSystem.h"
#include "glm/ext/vector_float3.hpp"
#include <functional>

namespace Saga {
    class GameWorld;
}

namespace Saga::Systems {
    /**
     * @brief Calls a function over the grid cells of a uniform grid that includes a certain bounding box.
     *
     * @param collisionSystemData the collision system data where the uniform grid is stored.
     * @param pos the position of the bounding box.
     * @param size of the bounding box.
     * @param callback the function to be called over all cells this bounding box intersects with.
     */
    inline void runOverGridCells(CollisionSystemData& collisionSystemData, glm::vec3 pos, glm::vec3 size, std::function<void(Entity)> callback);

    /**
     * @brief Add an entity to the uniform grid. Here, the entity must contain a cylinder collider 
     * and transform, and we add this entity to any grid cell that the cylinder specified by the collider and transform 
     * intersects.
     *
     * @param collisionSystemData the collision system data where the uniform grid is stored.
     * @param entity the entity.
     * @param cylinderCollider the cylinder collider. 
     * @param transform used to position this cylinder collider.
     */
    inline void addToUniformGrid(CollisionSystemData& collisionSystemData, Entity entity, CylinderCollider& cylinderCollider, Transform& transform);


    /**
     * @brief Remove an entity from the uniform grid. 
     * Here, the entity must contain a cylinder collider and transform, and we add this entity to any grid cell that the cylinder specified by the collider and transform intersects.
     *
     * @param collisionSystemData the collision system data where the uniform grid is stored.
     * @param entity the entity.
     * @param cylinderCollider the cylinder collider. 
     * @param transform used to position this cylinder collider.
     */
    inline void removeFromUniformGrid(CollisionSystemData& collisionSystemData, Entity entity, CylinderCollider& cylinderCollider, Transform& transform);

    /**
     * @brief Build a world's uniform grid from all objects with cylinder colliders.
     *
     * @param world
     */
    void rebuildUniformGrid(std::shared_ptr<GameWorld> world);


    /**
     * @brief Retrieve the closest dynamic collision to a cylinder.
     *
     * @param world
     * @param systemData the data for doing collision detection. This value is optional, and 
     *      if not specified, the function retrieves it from world.
     * @param entity the moving entity.
     * #param cylinderCollider the cylinder collider of the moving entity.
     * @param pos the starting position of the moving ellipsoid.
     * @param dir the direction (as well as distance) the ellipsoid moves.
     *
     * @return a Collision if the moving ellipsoid collide with static terrain.
     */
    std::optional<Collision> getClosestCollisionDynamic(std::shared_ptr<GameWorld> world, std::optional<CollisionSystemData*> systemData,
        Entity entity, CylinderCollider& cylinderCollider, glm::vec3 pos, glm::vec3 dir);
}

#include "collisionSystemOptimizationDynamic.inl"
