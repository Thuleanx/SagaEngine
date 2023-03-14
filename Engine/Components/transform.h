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

	Transform() { transform = std::make_shared<GraphicsEngine::ModelTransform>(); }
};
} // namespace Saga
