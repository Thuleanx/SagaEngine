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
		 * @brief Find the intersection time t where a ray and triangle intersects. Does not detect intersections to the back side of the triangle.
		 * 
		 * @param origin origin of the ray.
		 * @param rayDirection direction of the ray.
		 * @param a triangle first point in counter-clockwise order.
		 * @param b triangle second point in counter-clockwise order.
		 * @param c triangle third point in counter-clockwise order.
		 * @return float t such that origin + t * rayDirection is the intersection point between the ray and the triangle. If t does not exist, or is negative, then return -1.
		 */
		float rayTriangleIntersection(const glm::vec3& origin, const glm::vec3& rayDirection, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

		/**
		 * @brief Find the intersection time t between a unit sphere at the origin and a ray.
		 * 
		 * @param origin origin of the ray.
		 * @param rayDirection direction of the ray.
		 * @return float positive t such that origin + t * rayDirection is the intersection between the ray and the unit sphere, or -1 if no such value exists.
		 */
		float rayUnitSphereAtOriginIntersection(const glm::vec3& origin, const glm::vec3& rayDirection);

		/**
		 * @brief Find the intersection time t between a ray and an axis-aligned ellipsoid.
		 * 
		 * @param origin origin of the ray.
		 * @param rayDirection direction of the ray.
		 * @param position position of the ellipsoid.
		 * @param radius radius of the ellipsoid.
		 * @return float positive t such that origin + t * rayDirection is the intersection between the ray and the ellipsoid, or -1 if no such value exists.
		 */
		float rayEllipsoidOriginIntersection(const glm::vec3& origin, const glm::vec3& rayDirection, const glm::vec3& position, const glm::vec3& radius);

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
		float unitSphereTriangleCollision(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

		/**
		 * @brief Find the intersection time t between a moving ellipsoid and a triangle.
		 * 
		 * @param ellipsoidPos position of the ellipsoid.
		 * @param ellipsoidDir direction of the ellipsoid.
		 * @param ellipsoidRadius radius of the ellipsoid.
		 * @param a triangle first point in counter-clockwise order.
		 * @param b triangle second point in counter-clockwise order.
		 * @param c triangle third point in counter-clockwise order.
		 * @return float t such that ellipsoidPos + t * ellipsoidDir is the first time the ellipsoid intersects with the triangle. 
		 * 	If they already intersects, then this would be the time they exits. -1 if no such time exists. t is in the range [0,1] otherwise
		 */
		float ellipsoidTriangleCollision(const glm::vec3& ellipsoidPos, const glm::vec3& ellipsoidDir, 
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
