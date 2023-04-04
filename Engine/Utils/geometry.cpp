#include "geometry.h"
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <algorithm>
#include "../_Core/logger.h"
#include "../_Core/asserts.h"
#include "glm/gtx/string_cast.hpp"

#define f(i,a,b) for (int i = a; i < b; i++)

namespace Saga::Geometry {
	namespace {
		/**
		 * @brief Solve for the minimum t such that quadratic equation at^2 + bt + c = 0 is satisfied
		 * 
		 * @param a 
		 * @param b 
		 * @param c 
		 * @return float the minimum t, or -1 if it does not exist
		 */
		float solvePositiveQuadratic(float a, float b, float c) {
			// system is linear, we solve it with b and c
			if (!a) {
				// bx + c = 0
				if (!b && c) {
					// cannot be solved
					SWARN("Equation %f t^2 + %f t + %f = 0 does not have a solution. Returning -1.", a,b,c);
					return -1;
				} else if (!b) {
					SWARN("Equation %f t^2 + %f t + %f = 0 have infinitely many solutions. Returning 0.", a,b,c);
					return -1;
				}
				return -c/b < 0 ? -1 : -c/b;
			}
			float d = b*b - 4*a*c;
			if (d < 0) return -1;
			d = sqrt(d);

			float c1 = (-b-d) / (2*a), c2 = (-b+d) / (2*a);
			if (c1 > c2) std::swap(c1, c2);

			if (c2 < 0) return -1;
			// choose between the two roots
			return c1 < 0 ? c2 : c1;
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

	float rayTriangleIntersection(const glm::vec3& origin, const glm::vec3& rayDirection, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
		glm::vec3 normal = glm::cross(b-a, c-a);
		if (glm::dot(rayDirection, normal) <= 0) return -1; // if hitting backside, ignore
		float t = glm::dot(a - origin, normal) / glm::dot(rayDirection, normal);  // intersection point with triangle plane
		if (t < 0) return -1;
		glm::vec3 p = origin + t * rayDirection;
		float u = glm::dot(p, b-a), v = glm::dot(p, c-a); // convert to triangular coordinates to check if in triangle
		return (u >= 0 && u <= 1 && v >= 0 && v <= 1) ? t : -1;
	}

	float rayUnitSphereAtOriginIntersection(const glm::vec3& origin, const glm::vec3& rayDirection) {
		float a = glm::dot(rayDirection, rayDirection);
		float b = 2 * glm::dot(rayDirection, origin);
		float c = glm::dot(origin, origin) - 1;
		return solvePositiveQuadratic(a,b,c);
	}

	float rayEllipsoidOriginIntersection(const glm::vec3& origin, const glm::vec3& rayDirection, 
		const glm::vec3& position, const glm::vec3& radius) {

		return rayUnitSphereAtOriginIntersection((origin - position) / radius, rayDirection / radius);
	}

	float unitSphereEdgeCollision(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& c, const glm::vec3& d) {
		// this case is the same as if the sphere is a line segment, and the edge is a cylinder of size 1.
		// technically, the edge should be a capsule, but the endpoint cases are captured in sphere-point collision
		glm::vec3 a = pos, b = pos + dir;
		glm::vec3 badc = glm::cross(b-a, d-c), acdc = glm::cross(a-c,d-c);
		// intersection with an infinitely long cylinder
		float A = glm::dot(badc, badc), B = 2 * glm::dot(badc, acdc), C = glm::dot(acdc, acdc) - glm::dot(d-c, d-c);
		float t = solvePositiveQuadratic(A,B,C);
		if (t < 0 || t > 1) return -1;

		glm::vec3 P = t*dir + pos;
		float proj = glm::dot(P-c, d-c);
		// check if the intersection is at the part of the cylinder we care about
		if (proj > 0 && proj < glm::dot(d-c, d-c)) 	return t;
		else 										return -1;
	}

	float ellipsoidTriangleCollision(const glm::vec3& ellipsoidPos, const glm::vec3& ellipsoidDir, 
		const glm::vec3& ellipsoidRadius, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {

		if (!glm::dot(ellipsoidDir, ellipsoidDir)) {
			// if not moving, we can't find collision. Might as well return -1
			return -1;
		}

		if (!glm::dot(ellipsoidRadius, ellipsoidRadius)) SERROR("ellipsoidRadius is zero");
		if (glm::any(glm::isnan(ellipsoidDir))) SERROR("ellipsoidDir contains nan");
		if (glm::any(glm::isnan(ellipsoidPos))) SERROR("ellipsoidPos contains nan");
		if (glm::any(glm::isnan(a))) SERROR("trianglePos contains nan");
		if (glm::any(glm::isnan(b))) SERROR("trianglePos contains nan");
		if (glm::any(glm::isnan(c))) SERROR("trianglePos contains nan");
		
		return unitSphereTriangleCollision(ellipsoidPos / ellipsoidRadius, ellipsoidDir / ellipsoidRadius, 
			a / ellipsoidRadius,b / ellipsoidRadius,c / ellipsoidRadius);
	}

    std::optional<float> rayBoxCollision(glm::vec3 corner0, glm::vec3 corner1, glm::vec3 rayOrigin, glm::vec3 rayDir) {
        // grabbbed from Journal of Computer Graphics Techniques: https://www.jcgt.org/published/0007/03/04/paper-lowres.pdf
        glm::vec3 inverseRayDir = glm::vec3(1/rayDir.x, 1/rayDir.y, 1/rayDir.z);


        glm::vec3 t0 = (corner0 - rayOrigin);
        glm::vec3 t1 = (corner1 - rayOrigin);

        // since we might be dividing by 0, we first check for this and remedy this
        for (int dim = 0; dim < 3; dim++) {
            // if rayDir in that direction is 0, then we must have started inside the bounds of the box in that direction
            // therefore, we would expect our point's displacement in that direction to the box's top and bottom points to be 
            // of different sign, or 0
            if (!rayDir[dim] && (corner0[dim] - rayOrigin[dim]) * (corner1[dim] - rayOrigin[dim]) <= 0) {
                return {}; // return nothing if not prompted
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

    float unitSphereTriangleCollision(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        // interior collision
        float t = rayTriangleIntersection( pos - glm::normalize(glm::cross(b-a, c-a)), dir, a, b, c);
		if (t > 1) t = -1;
        SASSERT_MESSAGE(!std::isnan(t),
            "Unit sphere collision results in nan for interior collision case. Sphere has position %s and direction %s. Triangle located at %s, %s, %s",
			glm::to_string(pos).c_str(),
			glm::to_string(dir).c_str(),
			glm::to_string(a).c_str(),
			glm::to_string(b).c_str(),
			glm::to_string(c).c_str()
		);
        // edge collision
        if (t == -1) {
            glm::vec3 tri[3] = {a, b, c};
            for (int i = 0; i < 3; i++) {
                float tc = unitSphereEdgeCollision(pos, dir, tri[i], tri[(i+1)%3]);
                if (t == -1 || t > tc) t = tc;
            }
        }
        SASSERT_MESSAGE(!std::isnan(t), "Unit sphere collision results in nan for edge collision case. Sphere has position %s and direction %s. Triangle located at %s, %s, %s",
			glm::to_string(pos).c_str(),
			glm::to_string(dir).c_str(),
			glm::to_string(a).c_str(),
			glm::to_string(b).c_str(),
			glm::to_string(c).c_str()
		);
		// point collision
        if (t == -1) {
            for (glm::vec3 v : {a,b,c}) {
                float tc = rayUnitSphereAtOriginIntersection(v - pos, -dir);
				if (tc > 1) tc = -1;
                if (t == -1 || t > tc) t = tc;
            }
        }
        SASSERT_MESSAGE(!std::isnan(t), 
			"Unit sphere collision results in nan for point collision case. Sphere has position %s and direction %s. Triangle located at %s, %s, %s",
			glm::to_string(pos).c_str(),
			glm::to_string(dir).c_str(),
			glm::to_string(a).c_str(),
			glm::to_string(b).c_str(),
			glm::to_string(c).c_str()
		);
        return t;
    }


}
