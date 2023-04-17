#include "ellipsoid.h"
#include "triangle.h"
#include <glm/geometric.hpp>
#include "Engine/Utils/math.h"
#include "Engine/_Core/asserts.h"
#include "Engine/_Core/logger.h"
#include "glm/gtx/string_cast.hpp"

namespace Saga::Geometry {

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

std::optional<float> rayUnitSphereAtOriginIntersection(const glm::vec3& origin, const glm::vec3& rayDirection) {
    float a = glm::dot(rayDirection, rayDirection);
    float b = 2 * glm::dot(rayDirection, origin);
    float c = glm::dot(origin, origin) - 1;
    return Math::solvePositiveQuadratic(a,b,c);
}

std::optional<float> rayEllipsoidIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, 
    const glm::vec3& position, const glm::vec3& radius) {

    return rayUnitSphereAtOriginIntersection((rayOrigin - position) / radius, rayDirection / radius);
}

std::optional<float> unitSphereTriangleCollision(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    // interior collision
    std::optional<float> t = rayTriangleIntersection( pos - glm::normalize(glm::cross(b-a, c-a)), dir, Triangle{a, b, c});
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

}
