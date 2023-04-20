#pragma once

#include "Engine/Components/collider.h"
#include "Engine/Components/collisionSystemData.h"
#include "Engine/Entity/entity.h"
#include "Engine/Systems/collisionSystem.h"
#include <memory>

namespace Saga {
    class GameWorld;
}

namespace Saga::Systems {
    /**
     * @brief Retrieve the world's CollisionSystemData. This should live as
     * a component on an entity. If none exists, an entity with the component
     * will be created.
     *
     * @param world the world that the data exists on.
     *
     * @return a reference to the CollisionSystemData that is in this world.
     */
    CollisionSystemData& getSystemData(std::shared_ptr<GameWorld> world);

    /**
     * @brief This builds the bounding volume hierarchy for all static meshes
     * in the scene, and stores it inside of CollisionSystemData. This lives on an
     * entity inside the world, and will create an entity to contain CollisionSystemData
     * if one has not been created.
     *
     * @param world
     */
    void rebuildStaticBVH(std::shared_ptr<GameWorld> world);

    /**
     * @brief Retrieve the closest static collision to a moving ellipsoid.
     *
     * @param world
     * @param systemData the data for doing collision detection. This value is optional, and 
     *      if not specified, the function retrieves it from world.
     * @param entity the moving entity.
     * @param ellipsoidCollider the ellipsoidCollider of the entity.
     * @param pos the starting position of the moving ellipsoid.
     * @param dir the direction (as well as distance) the ellipsoid moves.
     *
     * @return a Collision if the moving ellipsoid collide with static terrain.
     */
    std::optional<Collision> getClosestCollisionStatic(std::shared_ptr<GameWorld> world, std::optional<CollisionSystemData*> systemData,
        Entity entity, EllipsoidCollider& ellipsoidCollider, glm::vec3 pos, glm::vec3 dir);
}
