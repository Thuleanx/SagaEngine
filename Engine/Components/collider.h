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
		glm::vec3 scale;

		EllipsoidCollider(glm::vec3 scale = glm::vec3(1,1,1)) : scale(scale) {}
	};
}
