#pragma once

#include "Graphics/modeltransform.h"
#include <memory>

namespace Saga {
/**
 * @brief A wrapper around a shared pointer to a ModelTransform.
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

	glm::vec3 getPos() const;
	glm::vec3 getUp() const;
	glm::vec3 getForward() const;
};
} // namespace Saga
