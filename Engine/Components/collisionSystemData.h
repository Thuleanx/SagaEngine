#pragma once

#include "Engine/Datastructures/Accelerant/bvh.h"

namespace Saga {

/**
 * @brief Manages data that the collision system uses. This lives in runtime on 
 * an empty entity.
 *
 * @ingroup component
 */
struct CollisionSystemData {
    std::optional<BoundingVolumeHierarchy> bvh; //!< the bounding volume hierarchy of triangles of static objectts
};

}
