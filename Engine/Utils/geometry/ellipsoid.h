#pragma once

#include <optional>
#include <glm/vec3.hpp>

namespace Saga {
    namespace Geometry {

        /**
         * @brief Find the intersection time t between a ray and an axis-aligned ellipsoid.
         * 
         * @param origin origin of the ray.
         * @param rayDirection direction of the ray.
         * @param position position of the ellipsoid.
         * @param radius radius of the ellipsoid.
         * @return std::optional<float> t in the range [0,1] such that origin + t * rayDirection is the intersection between the ray and the ellipsoid.
         * @return nothing if no such t exists.
         * @return 0 if ray starts out intersecting the the ellipsoid.
         *
         * @ingroup geometry
         */
        std::optional<float> rayEllipsoidIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& position, const glm::vec3& radius);

        /**
         * @brief Find the intersection time t between a unit sphere at the origin and a ray.
         * 
         * @param origin origin of the ray.
         * @param rayDirection direction of the ray.
         * @return float t in the range [0,1] such that origin + t * rayDirection is the intersection between the ray and the unit sphere. 
         * @return 0 if ray origin already intersect the unit sphere.
         * @return nothing if no intersection exists.
         *
         * @ingroup geometry
         */
        std::optional<float> rayUnitSphereAtOriginIntersection(const glm::vec3& origin, const glm::vec3& rayDirection);

        /**
         * @brief Find the intersection time t between a moving unit sphere with a triangle.
         * 
         * @param pos position of the unit sphere.
         * @param dir direction of the unit sphere.
         * @param a triangle first point in counter-clockwise order.
         * @param b triangle second point in counter-clockwise order.
         * @param c triangle third point in counter-clockwise order.
         * @return float positive t such that pos + t * dir is the first time the unit sphere intersects with the triangle. 
         * If they already intersects, then this would be the time they exits. -1 if no such time exists. t is in the range [0,1] otherwise
         *
         * @ingroup geometry
         */
        std::optional<float> unitSphereTriangleCollision(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

        /**
         * @brief Find the intersection between two moving axis-aligned ellipsoids.
         * 
         * @param ellipsoidPos0 position of the first ellipsoid.
         * @param ellipsoidDir0 direction that the first ellipsoid is moving.
         * @param ellipsoidRadius0 radius of the first ellipsoid.
         * @param ellipsoidPos1 position of the second ellipsoid.
         * @param ellipsoidRadius1 radius of the second ellipsoid.
         *
         * @ingroup geometry
         */
        std::optional<float> movingEllipsoidEllipsoidIntersection(
            const glm::vec3 &ellipsoidPos0, const glm::vec3 &ellipsoidDir0, const glm::vec3 &ellipsoidRadius0,
            const glm::vec3 &ellipsoidPos1, const glm::vec3 &ellipsoidRadius1);

		/**
		 * @brief Find the intersection time t between a moving ellipsoid and a triangle.
		 * 
		 * @param ellipsoidPos position of the ellipsoid.
		 * @param ellipsoidDir direction of the ellipsoid.
		 * @param ellipsoidRadius radius of the ellipsoid.
		 * @param a triangle first point in counter-clockwise order.
		 * @param b triangle second point in counter-clockwise order.
		 * @param c triangle third point in counter-clockwise order.
		 * @return float t in [0,1] such that ellipsoidPos + t * ellipsoidDir is the first time the ellipsoid intersects with the triangle.
         * @return float t in [0,1] such that ellipsoidPos * t * ellipsoidDir is the time the ellipsoid exits the triangle, should it already intersect at ellipsoidPos.
         * @return nothing if t does not exists, or is not in range, or the ellipsoid is not moving.
         *
         * @ingroup geometry
		 */
        std::optional<float> ellipsoidTriangleCollision(const glm::vec3& ellipsoidPos, const glm::vec3& ellipsoidDir, 
			const glm::vec3& ellipsoidRadius, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

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
         *
         * @ingroup geometry
         */
        std::optional<float> unitSphereEdgeCollision(const glm::vec3& pos, const glm::vec3& dir,
                const glm::vec3& c, const glm::vec3& d);
    }
}
