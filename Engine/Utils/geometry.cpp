#include "geometry.h"
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <algorithm>
#include "../_Core/logger.h"
#include "../_Core/asserts.h"
#include "glm/gtx/string_cast.hpp"
#include "Engine/Utils/math.h"

#define f(i,a,b) for (int i = a; i < b; i++)

namespace Saga::Geometry {
	namespace {

        /**
         * @brief Find collision between a moving unit sphere and an edge (line segment) in 3D.
         *
         * @param pos position of the sphere.
         * @param dir direction the sphere is moving in.
         * @param c an endpoint of the line segment.
         * @param d another endpoint of the line segment.
         *
         * @return float t in [0,1] such that pos + dir * t is when the sphere collide with the line segment.
         * @return float t in [0,1] such that pos + dir * t is when the sphere exits the line segment, if the sphere already collides at pos.
         * @return nothing if no such point exists, or none exists in the given range.
         */
        std::optional<float> unitSphereEdgeCollision(const glm::vec3& pos, const glm::vec3& dir, 
                const glm::vec3& c, const glm::vec3& d) {
            // this case is the same as if the sphere is a line segment, and the edge is a cylinder of size 1.
            // technically, the edge should be a capsule, but the endpoint cases are captured in sphere-point collision
            glm::vec3 a = pos, b = pos + dir;
            glm::vec3 badc = glm::cross(b-a, d-c), acdc = glm::cross(a-c,d-c);
            // intersection with an infinitely long cylinder
            float A = glm::dot(badc, badc), B = 2 * glm::dot(badc, acdc), C = glm::dot(acdc, acdc) - glm::dot(d-c, d-c);

            std::optional<float> t = Math::solvePositiveQuadratic(A, B, C);
            // if no valid solution, return nothing
            if (!t || t.value() > 1) 
                return {};

            glm::vec3 P = t.value()*dir + pos;
            float proj = glm::dot(P-c, d-c);
            // check if the intersection is at the part of the cylinder we care about
            bool intersecting = proj > 0 && proj < glm::dot(d-c, d-c);
            if (intersecting) return t.value();
            return {};
        }
	}

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
            float heightCombined = signedDir * (height0 + height1) / 2;

            lov = ((pos1.y - heightCombined) - pos0.y) / dir.y;
            hiv = ((pos1.y + heightCombined) - pos0.y) / dir.y;
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
            glm::vec3 displacement = pos1 - pos0;
            glm::vec2 rayOrigin = glm::vec2(displacement.x, displacement.z);

            auto result = rayUnitCircleAtOriginIntersection(rayOrigin, glm::vec2(dir.x, dir.z) / (radius0 + radius1));

            // if the circles never intersect
            if (!result) return {};

            std::tie(loxz, hixz) = result.value();
        }

        lov = std::max(0.f, lov);

        bool hasCollision = 1;
        hasCollision &= loxz <= hiv && lov <= hixz; // the intervals must intersect
        hasCollision &= loxz <= hixz && lov <= hiv; // the intervals must be valid
        hasCollision &= lov <= 1 || loxz <= 1; // at least one interval must intersect with [0,1]

        if (!hasCollision)
            return {};

        if (lov < loxz) {
            // vertical collision first, and the normal is in the opposite direction
            return std::make_tuple(lov, glm::vec3(0, dir.y < 0 ? 1 : -1, 0));
        } 

        // collision in the xz plane
        glm::vec3 projectedPos0 = pos0 + dir * loxz;
        glm::vec3 projectedNormal = projectedPos0 - pos1;

        return std::make_tuple(loxz, glm::normalize(glm::vec3(projectedNormal.x, 0, projectedNormal.z)));
    }

	float detectLineSegmentCollision(
		const float &alo, const float &ahi, 
		const float &blo, const float &bhi) {

		float aUp = bhi - alo, bDown = ahi - blo;
		if (aUp < 0 || bDown < 0) return 0;
		if (aUp < bDown) return aUp;
		return -bDown;
	}

	glm::vec2 detectCircleCollision(
		const glm::vec2& aPos, const float &aRadius, 
		const glm::vec2& bPos, const float &bRadius) {

		glm::vec2 dis = (aPos - bPos);
		float len2 = glm::dot(dis, dis);
		// no collision if the distance^2 is larger than sum of radius squared
		if (len2 >= (aRadius + bRadius) * (aRadius + bRadius)) 
			return glm::vec2(0,0);
		if (len2) {
			float len = sqrt(len2);
			return ((aRadius + bRadius) / len - 1.0f) * dis;
		} else return glm::vec2(aRadius + bRadius, 0); // if the objects has the same center, then mtv is to the right.
	}

    std::optional<float> rayTriangleIntersection(const glm::vec3& origin, const glm::vec3& rayDirection, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
		glm::vec3 normal = glm::cross(b-a, c-a);
        // if hitting backside or grazing, ignore
		if (glm::dot(rayDirection, normal) <= 0) 
            return {}; 
		float t = glm::dot(a - origin, normal) / glm::dot(rayDirection, normal);  // intersection point with triangle plane
		if (t < 0 || t > 1) 
            return {};
		glm::vec3 p = origin + t * rayDirection;
        // convert to triangular coordinates to check if in triangle
		float u = glm::dot(p, b-a), v = glm::dot(p, c-a);

        if (u >= 0 && u <= 1 && v >= 0 && v <= 1) return t;
        return {};
	}

    std::optional<float> rayUnitSphereAtOriginIntersection(const glm::vec3& origin, const glm::vec3& rayDirection) {
		float a = glm::dot(rayDirection, rayDirection);
		float b = 2 * glm::dot(rayDirection, origin);
		float c = glm::dot(origin, origin) - 1;
		return Math::solvePositiveQuadratic(a,b,c);
	}

    std::optional<std::tuple<float,float>> rayUnitCircleAtOriginIntersection(const glm::vec2& origin, const glm::vec2& rayDirection) {
		float a = glm::dot(rayDirection, rayDirection);
		float b = 2 * glm::dot(rayDirection, origin);
		float c = glm::dot(origin, origin) - 1;
        std::vector<float> sols = Math::solveQuadraticReals(a,b,c);
        // this ensures that the resulting tlo and thi are ordered
        if (sols.size() == 2 && sols[0] > sols[1]) 
            std::swap(sols[0], sols[1]);
        // no collision
        if (!sols.size()) 
            return {};
        if (sols.size() == 1)
            return std::make_tuple(sols[0], sols[0]);
        return std::make_tuple(sols[0], sols[1]);
    }

    std::optional<float> rayEllipsoidIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, 
		const glm::vec3& position, const glm::vec3& radius) {

		return rayUnitSphereAtOriginIntersection((rayOrigin - position) / radius, rayDirection / radius);
	}

    std::optional<float> ellipsoidTriangleCollision(const glm::vec3& ellipsoidPos, const glm::vec3& ellipsoidDir, 
		const glm::vec3& ellipsoidRadius, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {

		if (!glm::dot(ellipsoidDir, ellipsoidDir)) {
			// if not moving, we can't find collision. Might as well return nothing
			return {};
		}

        // TODO: compile these out of the release build
		if (!glm::dot(ellipsoidRadius, ellipsoidRadius)) SERROR("ellipsoidRadius is zero");
		if (glm::any(glm::isnan(ellipsoidDir))) SERROR("ellipsoidDir contains nan");
		if (glm::any(glm::isnan(ellipsoidPos))) SERROR("ellipsoidPos contains nan");
		if (glm::any(glm::isnan(a))) SERROR("trianglePos contains nan");
		if (glm::any(glm::isnan(b))) SERROR("trianglePos contains nan");
		if (glm::any(glm::isnan(c))) SERROR("trianglePos contains nan");
		
		return unitSphereTriangleCollision(ellipsoidPos / ellipsoidRadius, ellipsoidDir / ellipsoidRadius, 
			a / ellipsoidRadius,b / ellipsoidRadius,c / ellipsoidRadius);
	}

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

    std::optional<float> unitSphereTriangleCollision(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        // interior collision
        std::optional<float> t = rayTriangleIntersection( pos - glm::normalize(glm::cross(b-a, c-a)), dir, a, b, c);
        SASSERT_MESSAGE(!t || !std::isnan(t.value()),
            "Unit sphere collision results in nan for interior collision case. Sphere has position %s and direction %s. Triangle located at %s, %s, %s",
			glm::to_string(pos).c_str(),
			glm::to_string(dir).c_str(),
			glm::to_string(a).c_str(),
			glm::to_string(b).c_str(),
			glm::to_string(c).c_str()
		);
        // edge collision
        if (!t) {
            glm::vec3 tri[3] = {a, b, c};
            for (int triangleIndex = 0; triangleIndex < 3; triangleIndex++) {
                std::optional<float> tc = unitSphereEdgeCollision(pos, dir, tri[triangleIndex], tri[(triangleIndex+1)%3]);
                if (!t || (tc && t.value() > tc.value())) t = tc;
            }
        }
        SASSERT_MESSAGE(!t || !std::isnan(t.value()), "Unit sphere collision results in nan for edge collision case. Sphere has position %s and direction %s. Triangle located at %s, %s, %s",
			glm::to_string(pos).c_str(),
			glm::to_string(dir).c_str(),
			glm::to_string(a).c_str(),
			glm::to_string(b).c_str(),
			glm::to_string(c).c_str()
		);
		// point collision
        if (!t) {
            for (glm::vec3 v : {a,b,c}) {
                std::optional<float> tc = rayUnitSphereAtOriginIntersection(v - pos, -dir);
                if (tc) {
                    if (tc.value() > 1) continue;
                    if (!t || tc.value() < t.value()) t = tc;
                }
            }
        }
        SASSERT_MESSAGE(!t || !std::isnan(t.value()),
			"Unit sphere collision results in nan for point collision case. Sphere has position %s and direction %s. Triangle located at %s, %s, %s",
			glm::to_string(pos).c_str(),
			glm::to_string(dir).c_str(),
			glm::to_string(a).c_str(),
			glm::to_string(b).c_str(),
			glm::to_string(c).c_str()
		);
        return t;
    }

    std::optional<float> movingEllipsoidEllipsoidIntersection(
        const glm::vec3 &ellipsoidPos0, const glm::vec3 &ellipsoidDir0, const glm::vec3 &ellipsoidRadius0,
        const glm::vec3 &ellipsoidPos1, const glm::vec3 &ellipsoidRadius1) {

        return rayEllipsoidIntersection(
            ellipsoidPos0, ellipsoidDir0,
            ellipsoidPos1, ellipsoidRadius0 + ellipsoidRadius1);
    }
}
