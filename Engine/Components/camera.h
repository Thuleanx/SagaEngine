#pragma once

#include "Engine/Utils/graphics/frustum.h"
#include "Graphics/camera.h"
#include <memory>
#include <glm/vec3.hpp>
#include <string>

namespace Saga {

/**
 * @brief Wrapper around the GraphicsEngine camera.
 * @ingroup component
 */
struct Camera {
	std::shared_ptr<GraphicsEngine::Camera> camera;
	bool clearColorBufferOnDraw; //!< determine if the draw system should clear the color buffer (or only the depth buffer) when using this camera.
	glm::vec3 clearColor; //!< only relevant if clearColorBufferOnDraw is true.
	glm::vec3 globalShaderCoefs; //!< only relevant if using phong shading. Coefficients for ambient, diffuse, and specular respectively. Must all be in the range [0,1].
	std::string shader; //!< shader used to render Meshes seen by this camera.

	operator const std::shared_ptr<GraphicsEngine::Camera>() const { return camera; }

    /**
     * @brief Get the world space frustum of the camera.
     *
     * @return the world space frustum of the camera, 
     * if it exists.
     * @return nothing if camera missing.
     */
    std::optional<Geometry::Frustum> getFrustum();
};

} // namespace Saga
