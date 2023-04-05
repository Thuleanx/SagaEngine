#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <optional>

namespace Saga {
	namespace Geometry {
		// 3D
		/**
		 * @brief Find the minimum translation vector for two axis-aligned cylinders. If the two cylinders are not penetrating, then return the zero vector.
		 * 
		 * @param height0 height of the first cylinder.
		 * @param radius0 radius of the first cylinder.
		 * @param pos0 position of the first cylinder.
		 * @param height1 height of the second cylinder.
		 * @param radius1 radius of the second cylinder.
		 * @param pos1 position of the second cylinder.
		 * @return glm::vec3 The minimum translation vector.
		 */
		glm::vec3 detectAACylinderCylinderCollision(float height0, float radius0, glm::vec3 pos0, float height1, float radius1, glm::vec3 pos1);

        /**
         * @brief Find the earliest time between intersection of two cylinders, one of which is moving.
         * It must be the case that the two cylinders are not penetrating at the start. 
         *
		 * @param height0 height of the first cylinder.
		 * @param radius0 radius of the first cylinder.
		 * @param pos0 position of the first cylinder.
		 * @param height1 height of the second cylinder.
		 * @param radius1 radius of the second cylinder.
		 * @param pos1 position of the second cylinder.
         * @param dir the direction of the first cylinder.
         * 
         * @return float t in (0,1] such that pos0 + t * dir is the first time that the two cylinders intersect.
         * @return glm::vec3 normal the normal direction of the contact.
         * @return nothing if the two cylinders are already penetrating, or if t does not exist in the range (0,1].
         * @warn does not work on degenerate cylinders. Will return nothing.
         */
        std::optional<std::tuple<float, glm::vec3>> movingCylinderCylinderIntersection(float height0, float radius0, glm::vec3 pos0, float height1, float radius1, glm::vec3 pos1, glm::vec3 dir);

		/**
		 * @brief Find the intersection time t where a ray and triangle intersects. Does not detect intersections to the back side of the triangle.
		 * 
		 * @param origin origin of the ray.
		 * @param rayDirection direction of the ray.
		 * @param a triangle first point in counter-clockwise order.
		 * @param b triangle second point in counter-clockwise order.
		 * @param c triangle third point in counter-clockwise order.
         *
		 * @return float t in [0,1] such that origin + t * rayDirection is the intersection point between the ray and the triangle. 
         * @return nothing if t does not exist, or is not in range, or is grazing (parallel to the triangle).
         * @return 0 if ray starts out intersecting the triangle.
		 */
        std::optional<float> rayTriangleIntersection(const glm::vec3& origin, const glm::vec3& rayDirection, 
                const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

		/**
		 * @brief Find the intersection time t between a unit sphere at the origin and a ray.
		 * 
		 * @param origin origin of the ray.
         * @param rayDirection direction of the ray.
         * @return float t in the range [0,1] such that origin + t * rayDirection is the intersection between the ray and the unit sphere. 
         * @return 0 if ray origin already intersect the unit sphere.
         * @return nothing if no intersection exists.
		 */
        std::optional<float> rayUnitSphereAtOriginIntersection(const glm::vec3& origin, const glm::vec3& rayDirection);

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
		 */
        std::optional<float> rayEllipsoidIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& position, const glm::vec3& radius);

        /**
         * @brief Find the intersection between two moving axis-aligned ellipsoids.
         * 
         * @param ellipsoidPos0 position of the first ellipsoid.
         * @param ellipsoidDir0 direction that the first ellipsoid is moving.
         * @param ellipsoidRadius0 radius of the first ellipsoid.
         * @param ellipsoidPos1 position of the second ellipsoid.
         * @param ellipsoidRadius1 radius of the second ellipsoid.
         */
        std::optional<float> movingEllipsoidEllipsoidIntersection(
            const glm::vec3 &ellipsoidPos0, const glm::vec3 &ellipsoidDir0, const glm::vec3 &ellipsoidRadius0,
            const glm::vec3 &ellipsoidPos1, const glm::vec3 &ellipsoidRadius1);

		/**
		 * @brief Find the intersection time t between a moving unit sphere with a triangle.
		 * 
		 * @param pos position of the unit sphere.
		 * @param dir direction of the unit sphere.
		 * @param a triangle first point in counter-clockwise order.
		 * @param b triangle second point in counter-clockwise order.
		 * @param c triangle third point in counter-clockwise order.
		 * @return float positive t such that pos + t * dir is the first time the unit sphere intersects with the triangle. 
		 * 	If they already intersects, then this would be the time they exits. -1 if no such time exists. t is in the range [0,1] otherwise
		 */
        std::optional<float> unitSphereTriangleCollision(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

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
		 */
        std::optional<float> ellipsoidTriangleCollision(const glm::vec3& ellipsoidPos, const glm::vec3& ellipsoidDir, 
			const glm::vec3& ellipsoidRadius, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

        /**
         * @brief Find the intersection time t between a ray and a box
         *
         * @param rayOrigin origin of the ray.
         * @param rayDir direction of the ray
         * @param cornerbotLeft bottom left corner of the box.
         * @param cornerTopRIght upper right corner of the box.
         * @return either the time t in [0,1] where rayOrigin + rayDir * t is intersecting the box, or nothing. If the ray starts inside the box, this return 0.
         */
        std::optional<float> rayBoxCollision(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 cornerBotLeft, glm::vec3 cornerTopRight);

		// 2D
		/**
		 * @brief Detect if two circles overlap. If they do, output the minimum translation vector from the first circle for both to not be colliding.
		 * 
		 * @param aPos center of the first circle.
		 * @param aRadius radius of the first circle.
		 * @param bPos center of the second circle.
		 * @param bRadius radius of the second circle.
		 * @return glm::vec2 zero if non overlapping. The minimum translation vector with respect to the first circle otherwise.
		 */
		glm::vec2 detectCircleCollision(const glm::vec2& aPos, const float &aRadius, const glm::vec2& bPos, const float &bRadius);

		/**
		 * @brief Find the entrance and exit times t between a unit circle at the origin and a ray.
		 * 
		 * @param origin origin of the ray.
         * @param rayDirection direction of the ray.
         * @return tlo <= thi such that origin + tlo * rayDirection is the intersection between the ray and the unit circle, 
         *  and the same applies to thi.
         * @return nothing if no intersection exists.
		 */
        std::optional<std::tuple<float,float>> rayUnitCircleAtOriginIntersection(const glm::vec2& origin, const glm::vec2& rayDirection);

		// 1D
		/**
		 * @brief Detect collision (and return minimum translation vector) of two line segments. 
		 * 
		 * @param alo starting point of the first line segment.
		 * @param ahi ending point of the first line segment. Must be at least the starting point.
		 * @param blo starting point of the second line segment.
		 * @param bhi ending point of the second line segment. Must be at least the starting point.
		 * @return float minimum distance the first line segment has to move for both to not be intersecting
		 */
		float detectLineSegmentCollision(const float &alo, const float &ahi, const float &blo, const float &bhi);

	}
}
