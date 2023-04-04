#pragma once

#include <glm/glm.hpp>
#include <optional>
#include "Engine/Entity/entity.h"

namespace Saga {

    /**
     * @brief Represents a bounding box.
     */
    struct BoundingBox {
        glm::vec3 bounds[2]; //!< coordinates of the lower-left and upper-right corners of the bounding box, respectively.

        // TODO: Cache these operations, with a dirty mask that tells you when bounds was altered (for recomputing these values)
        /**
         * @brief Compute the centroid of the BoundingBox.
         * @return glm::vec3 centroid of the BoundingBox.
         */
        glm::vec3 centroid();

        /**
         * @brief Compute the size of the BoundingBox.
         * @return glm::vec3 size of the BoundingBox in each direction.
         */
        glm::vec3 size();


        /**
         * @brief Compute the surface area of the BoundinBox.
         * @return float the surface area. 0 if the BoundingBox is not valid, for 
         * instance if therte exists a dimension dim such that bounds[0][dim] > bounds[1][dim]
         */
        float surfaceArea();

        /**
         * @brief Compute the volume of the BoundingBox.
         * @return float volume of the BoundingBox. If the BoundingBox 
         * is not valid, for instance if there exists a dimension dim
         *  such that bounds[0][dim] > bounds[1][dim] then this value is 0.
         */
        float volume();

        /**
         * @brief Determins if a point is inside of the bounding box
         * @return true if it is inside.
         * @return false otherwise.
         */
        bool inside(glm::vec3 point);

        /**
         * @brief Determine if the BoundingBox collide with a ray.
         * @param point the start of the ray.
         * @param dir the direction of the ray.
         * @return true if the ray collides with the bounding box.
         * @return false otherwise.
         */
        bool collidesWithRay(glm::vec4 point, glm::vec4 dir);

        /**
         * @brief Find the point of collision between a ray and the BoundingBox.
         * @param point the start of the ray.
         * @param dir the direction of the ray.
         * @return std::optional<float> t where point + dir * t is the first collision between the ray
         * and the bounding box. t is in the range [0,1]. If the ray starts inside of the 
         * BoundingBox, then return 0.
         */
        std::optional<float> findCollisionWithRay(glm::vec4 point, glm::vec4 dir);

        /**
         * @brief Determines if a BoundingBox intersects with another.
         * @param other the other bounding box.
         * @return true if the two intersects.
         * @return false otherwise.
         */
        bool intersectWith(const BoundingBox &other);

        /**
         * @brief Compute the Lower-Bound squared distance from a point to the BoundingBox.
         * @param point the point to compute distance to.
         * @return a number that represents a lower-bound on the squared distance to the box.
         */
        float lowerBoundSquareDistanceTo(const glm::vec3 point);

        /**
         * @brief Find the collision between a moving bounding box and this one.
         *
         * @param centroid centroid of the box.
         * @param dir direction the box is moving.
         * @param size the size of the box.
         * @return std::optional<float> t where centroid+dir*t is the first collision between the 
         *  box and bounding box, if it exists. This value is in the range [0,1]. If the boxes
         *  start off intersecting, the value returned will be 0.
         */
        std::optional<float> findCollisionWithBox(glm::vec3 centroid, glm::vec3 dir, glm::vec3 size);

        /**
         * @brief Get a special invalid BoundingBox used as a starting point for mergeBox
         * and confineBox. This bounding box has the lower left corner at infinitiy and upper right corner at negative
         * infinity.
         */
        static BoundingBox getExtremeBound();

        /**
         * @brief Compute a BoundingBox that is the union of the two input boxes.
         * @param a the first bounding box.
         * @param b the second bounding box.
         * @return BoundingBox the union of the two.
         */
        static BoundingBox mergeBox(const BoundingBox &a, const BoundingBox &b);

        /**
         * @brief Confine a BoundingBox within the other, effectively computing their intersection.
         * @param a the first bounding box.
         * @param b the second bounding box.
         * @return BoundingBox the intersection of the two.
         */
        static BoundingBox confineBox(const BoundingBox &a, const BoundingBox &b);

        /**
         * @brief Create a bounding box containing only one point.
         * @param point the point the bounding box contains.
         * @return BoundingBox a bounding box containing only that point.
         */
        static BoundingBox pointBox(const glm::vec3 &point);
    };


};
