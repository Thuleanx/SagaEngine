#pragma once

#include "Graphics/modeltransform.h"
#include <memory>

namespace Saga {
/**
 * @brief A wrapper around a shared pointer to a ModelTransform.
 * This tells object where to position themselves in a 3D scene.
 * @ingroup component
 */
struct Transform {
	std::shared_ptr<GraphicsEngine::ModelTransform> transform;

	operator const std::shared_ptr<GraphicsEngine::ModelTransform> &() const { return transform; }

	/**
	 * @brief Construct a new Transform object.
	 * @note this requests the graphics engine for a model transform.
	 */
	Transform();

	/**
	 * @return glm::vec3 the position of the transform.
	 */
	glm::vec3 getPos() const;

	/**
	 * @return glm::vec3 the up vector of the transform.
	 */
	glm::vec3 getUp() const;

	/**
	 * @return glm::vec3 the forward vector of the transform.
	 */
	glm::vec3 getForward() const;
};
} // namespace Saga
