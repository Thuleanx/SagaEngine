#pragma once

#include "Engine/Datastructures/Accelerant/bvh.h"
#include "Engine/Datastructures/Accelerant/uniformGrid.h"

namespace Saga {

/**
 * @brief Manages data that the collision system uses. This lives in runtime on 
 * an empty entity.
 *
 * @ingroup component
 */
struct CollisionSystemData {
    std::optional<BoundingVolumeHierarchy> bvh; //!< the bounding volume hierarchy of triangles of static objectts.
    std::optional<UniformGrid<Entity>> uniformGrid; //!< the uniform grids used in optimizing dynamic-dynamic collisions.
};

}
