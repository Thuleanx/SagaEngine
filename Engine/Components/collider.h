#pragma once
#include <glm/vec3.hpp>
#include <vector>
#include <tuple>

namespace Saga {
	/**
	 * @brief A boolean flag to tell if an Entity has a Collider.
	 * @ingroup component
	 */
	struct Collider { };

	/**
	 * @brief Model an axis-aligned cylinder collider.
	 * @ingroup component
	 */
	struct CylinderCollider {
		float height;
		float radius;

        /**
         * Create a cylinder collider.
         *
         * @param height the height of the cylinder collider.
         * @param radius of the cylinder collider.
         */
		CylinderCollider(float height = 1, float radius = 0.5f) : height(height), radius(radius) {}
	};

	/**
	 * @brief Model a mesh containing a list of triangles.
	 * @ingroup component
	 */
    struct MeshCollider { };

	/**
	 * @brief Model an axis-aligned ellipsoid.
	 * @ingroup component
	 */
	struct EllipsoidCollider {
		glm::vec3 radius;

        /**
         * @brief Construct an ellipsoid collider.
         *
         * @param radius the radius of the ellipsoid collider. So its bounding box will be twice the size of this radius.
         */
		EllipsoidCollider(glm::vec3 radius = glm::vec3(1,1,1)) : radius(radius) {}
	};
}
