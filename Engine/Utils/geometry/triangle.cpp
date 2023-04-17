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
        // if hitting backside or grazing, ignore
		if (glm::dot(rayDirection, normal) >= 0)
            return {};
		float t = glm::dot(triangle.a - origin, normal) / glm::dot(rayDirection, normal);  // intersection point with triangle plane
		if (t < 0 || t > 1)
            return {};
		glm::vec3 p = origin + t * rayDirection;
        // convert to triangular coordinates to check if in triangle
		float u = glm::dot(p - triangle.a, uDir), v = glm::dot(p - triangle.a, vDir);

        if (u >= 0 && u <= glm::dot(uDir, uDir) && v >= 0 && v <= glm::dot(vDir, vDir)) return t;
        return {};
	}

    glm::vec3 getClosestPoint(const glm::vec3 pointPos, Triangle triangle) {
        glm::vec3 barycentricPos = triangle.toBarycentric(pointPos);
        barycentricPos[0] = std::clamp(barycentricPos[0], 0.f, 1.f);
        barycentricPos[1] = std::clamp(barycentricPos[1], 0.f, 1.f);
        return barycentricPos[0] * (triangle.b - triangle.a) + barycentricPos[1] * (triangle.c - triangle.a) + triangle.a;
    }
}
