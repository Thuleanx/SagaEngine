#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <optional>

namespace Saga::Geometry {
    /**
        * @brief Represents a 3D triangle, equipped with some utility functions
        */
    struct Triangle {
        glm::vec3 a,b,c;

        std::optional<glm::vec3> normal_cached;

        /**
         * @brief Convert a point from cartesian to barycentric coordinate with
         * respect to the current triangle.
         *
         * @param pos the position in cartesian space.
         * @return glm::vec3 the barycentric coordinate (u,v,w) such that p projected onto
         * the plane of a,b,c is = u * a + v * b + w * c.
         */
        glm::vec3 toBarycentric(const glm::vec3 pos);

        /**
         * @brief Convert a point from cartesian to UV coordinate
         *
         * @param pos the position in cartesian space.
         * @return glm::vec3 the barycentric coordinate (u,v,w) such that p = u * a + v * b + w * c.
         */
        glm::vec2 toUV(const glm::vec3 pos);

        /**
         * @brief Get the normal of this triangle.
         * @return glm::vec3 the normal of this triangle in the direction of cross(b-a, c-a).
         */
        glm::vec3 getNormal();
    };

    /**
     * @brief Find the intersection time t where a ray and triangle intersects. Does not detect intersections to the back side of the triangle.
     * 
     * @param origin origin of the ray.
     * @param rayDirection direction of the ray.
     * @param triangle the triangle.
     *
     * @return float t in [0,1] such that origin + t * rayDirection is the intersection point between the ray and the triangle. 
     * @return nothing if t does not exist, or is not in range, or is grazing (parallel to the triangle).
     * @return 0 if ray starts out intersecting the triangle.
     */
    std::optional<float> rayTriangleIntersection(const glm::vec3& origin, const glm::vec3& rayDirection, Triangle triangle);

    /**
     * @brief Find the closest point between a point and a triangle.
     *
     * @param pointPos position of the point in cartesian coordinate.
     * @param triangle the triangle.
     *
     * @return glm::vec3 a point inside the triangle whose euclidean distance is cloest to the pointPos.
     */
    glm::vec3 getClosestPoint(const glm::vec3 pointPos, Triangle triangle);
}
