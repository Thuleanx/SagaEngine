#include "box.h"

#include <glm/geometric.hpp>
#include <algorithm>

namespace Saga::Geometry {
    std::optional<float> rayBoxCollision(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 corner0, glm::vec3 corner1) {
        // grabbbed from Journal of Computer Graphics Techniques: https://www.jcgt.org/published/0007/03/04/paper-lowres.pdf
        glm::vec3 inverseRayDir = glm::vec3(1/rayDir.x, 1/rayDir.y, 1/rayDir.z);

        glm::vec3 t0 = (corner0 - rayOrigin);
        glm::vec3 t1 = (corner1 - rayOrigin);

        // since we might be dividing by 0, we first check for this and remedy this
        for (int dim = 0; dim < 3; dim++) {
            // if rayDir in that direction is 0, then we must have started inside the bounds of the box in that direction
            // therefore, we would expect our point's displacement in that direction to the box's top and bottom points to be 
            // of different sign, or 0
            if (!rayDir[dim] && (corner0[dim] - rayOrigin[dim]) * (corner1[dim] - rayOrigin[dim]) > 0) {
                return {}; // return nothing if not going to intersect
            } else if (!rayDir[dim]) {
                // this means that in this dimension, we stay inside of the bounds for the entire duration
                t0[dim] = 0;
                t1[dim] = 1;
            } else {
                t0[dim] *= inverseRayDir[dim];
                t1[dim] *= inverseRayDir[dim];
            }
        }

        glm::vec3 tmin = glm::min(t0, t1), tmax = glm::max(t0, t1);

        // to guarantee the output is in the [0,1] range, we take max and min with respect to that range
        float lo = std::max(std::max(0.0f, tmin[0]), std::max(tmin[1], tmin[2]));
        float hi = std::min(std::min(1.0f, tmax[0]), std::min(tmax[1], tmax[2]));

        if (lo > hi) return {};
        return lo; // should be the first time that the ray intersects the box
    }
}
