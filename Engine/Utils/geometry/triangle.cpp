#include "triangle.h"
#include <algorithm>
#include <glm/geometric.hpp>

namespace Saga::Geometry {

    glm::vec3 Triangle::toBarycentric(const glm::vec3 pos) {
        glm::vec3 v0 = b-a, v1 = c-a, v2 = pos-a;

        // TODO: cache some of these values
        float d00 = glm::dot(v0, v0);
        float d01 = glm::dot(v0, v1);
        float d11 = glm::dot(v1, v1);
        float d20 = glm::dot(v2, v0);
        float d21 = glm::dot(v2, v1);
        float denom = d00 * d11 - d01 * d01;

        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;

        return glm::vec3(u,v,w);
    }

    glm::vec2 Triangle::toUV(const glm::vec3 pos) {
        glm::vec3 barycentric = toBarycentric(pos);
        return glm::vec2(barycentric);
    }

    glm::vec3 Triangle::getNormal() {
        // boils down to normalize((b-a) x (c-a))
        // we cache it because computation of sqrt can get expensive
        if (normal_cached) return normal_cached.value();
        return (normal_cached = glm::normalize(glm::cross(b-a, c-a))).value();
    }

    std::optional<float> rayTriangleIntersection(const glm::vec3& origin, const glm::vec3& rayDirection, Triangle triangle) {
        glm::vec3 uDir = triangle.b-triangle.a, vDir = triangle.c-triangle.a;
		glm::vec3 normal = glm::cross(uDir, vDir);

        // degenerate triangle, we dont accept collisions
        if (glm::dot(normal, normal) == 0) 
            return {};

        float displacementAlongNormal = glm::dot(triangle.a - origin, normal);
        float alignmentNormal = glm::dot(rayDirection, normal);

        // ray direction is parallel to the plane
        if (abs(alignmentNormal) <= 0.0001) {
            // ray lies in a different plane
            if (displacementAlongNormal) return {};
            else {
                // ray lies in the same plane. We can ignore this degenerate case for now
                return {};
            }
        }
        // if hitting backside or grazing, ignore
		if (alignmentNormal >= 0)
            return {};

		float t = displacementAlongNormal / alignmentNormal;  // intersection point with triangle plane
		if (t < 0 || t > 1)
            return {};
		glm::vec3 p = origin + t * rayDirection;

        // convert to barycentric coordinates to check if in triangle
        glm::vec3 uvw = triangle.toBarycentric(p);
        float u = uvw.x, v = uvw.y;

        if (u >= 0 && v >= 0 && u + v <= 1) return t;
        return {};
	}

    glm::vec3 getClosestPoint(const glm::vec3 pointPos, Triangle triangle) {
        glm::vec3 barycentricPos = triangle.toBarycentric(pointPos);
        barycentricPos[0] = std::clamp(barycentricPos[0], 0.f, 1.f);
        barycentricPos[1] = std::clamp(barycentricPos[1], 0.f, 1.f);
        float tot = barycentricPos[0] + barycentricPos[1];
        if (tot > 1) { // if still outside the triangle after the clamp, we scale down
            barycentricPos[0] /= tot;
            barycentricPos[1] /= tot;
        }
        return barycentricPos[0] * (triangle.b - triangle.a) + barycentricPos[1] * (triangle.c - triangle.a) + triangle.a;
    }
}
