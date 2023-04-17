#include "cylinder.h"
#include <glm/geometric.hpp>
#include <algorithm>
#include "circle.h"
#include "line.h"

namespace Saga::Geometry {
	glm::vec3 detectAACylinderCylinderCollision(
		float height0, float radius0, glm::vec3 pos0, 
		float height1, float radius1, glm::vec3 pos1) {

		// detect vertical minimum translation vector
		float vmtv = detectLineSegmentCollision(pos0.y - height0/2, pos0.y + height0/2, 
			pos1.y - height1/2, pos1.y + height1/2);
		// circle-circle collision for the base of the cylinders
		glm::vec2 hmtv = detectCircleCollision(glm::vec2(pos0.x,pos0.z), radius0, 
			glm::vec2(pos1.x,pos1.z), radius1);

		if (vmtv * vmtv > glm::dot(hmtv, hmtv)) 
			return glm::vec3(hmtv.x, 0, hmtv.y);
		return glm::vec3(0, vmtv, 0);
	}

    std::optional<std::tuple<float, glm::vec3>> movingCylinderCylinderIntersection(
        float height0, float radius0, glm::vec3 pos0, 
        float height1, float radius1, glm::vec3 pos1, glm::vec3 dir) {

        if (!radius0 || !radius1) return {};

        // strategy is to find intervals where the two cylinders overlap in the 
        // y axis and in the xz plane. Then the low point of the two intervals,
        // assuming the intersection is valid, is the desired time t

        float lov, hiv;
        // if not moving in the y direction
        if (!dir.y) {
            float vmtv = detectLineSegmentCollision(pos0.y - height0/2, pos0.y + height0/2, 
                pos1.y - height1/2, pos1.y + height1/2);
            // if the two line segments don't ever intersect, we can say there's no collision
            if (!vmtv) return {};
            lov = 0, hiv = 1;
        } else {
            float signedDir = dir.y < 0 ? -1 : 1;
            float heightCombined = (height0 + height1) / 2;

            lov = ((pos1.y - heightCombined) - pos0.y) / dir.y;
            hiv = ((pos1.y + heightCombined) - pos0.y) / dir.y;
            if (lov > hiv) std::swap(lov, hiv);
        }
        
        float loxz, hixz;
        if (!dir.x && !dir.z) {
            glm::vec2 hmtv = detectCircleCollision(glm::vec2(pos0.x,pos0.z), radius0, 
                glm::vec2(pos1.x,pos1.z), radius1);
            // if two circles never intersect, then we say there's no collision
            if (!hmtv.x && !hmtv.y) return {};
            loxz = 0, hixz = 1;
        } else {
            // for solving quadratic formula
            glm::vec3 displacement = pos0 - pos1;
            glm::vec2 rayOrigin = glm::vec2(displacement.x, displacement.z);

            auto result = rayUnitCircleAtOriginIntersection(rayOrigin, glm::vec2(dir.x, dir.z) / (radius0 + radius1));

            // if the circles never intersect
            if (!result) return {};

            std::tie(loxz, hixz) = result.value();
        }

        lov = std::max(0.f, lov);
        loxz = std::max(0.f, loxz);

        bool hasCollision = 1;
        hasCollision &= loxz <= hiv && lov <= hixz; // the intervals must intersect
        hasCollision &= loxz <= hixz && lov <= hiv; // the intervals must be valid
        hasCollision &= lov <= 1 && loxz <= 1; // at least one interval must intersect with [0,1]

        if (!hasCollision)
            return {};

        if (lov > loxz) {
            // vertical collision first, and the normal is in the opposite direction
            return std::make_tuple(lov, glm::vec3(0, dir.y < 0 ? 1 : -1, 0));
        } 

        // collision in the xz plane
        glm::vec3 projectedPos0 = pos0 + dir * loxz;
        glm::vec3 projectedNormal = projectedPos0 - pos1;

        return std::make_tuple(loxz, glm::normalize(glm::vec3(projectedNormal.x, 0, projectedNormal.z)));
    }
}
