#pragma once

#include "Graphics/light.h"
#include <memory>

namespace Saga {

/**
 * @brief Model a light. Wrapper around GraphicsEngine's light. Currently only support Point and Directional Lights.
 * @ingroup component
 */
struct Light {
	std::shared_ptr<GraphicsEngine::Light> light;

	operator const std::shared_ptr<GraphicsEngine::Light> &() const { return light; }

	/**
	 * @brief Construct a new Light object. Should not be used.
	 */
	Light();

	/**
	 * @brief Construct a new Light object of a certain type.
	 * 
	 * @param type type of the light. Can be GraphicsEngine::LightType::Directional or GraphicsEngine::LightType::Point
	 * @param lightData if the light is directional, this is the normalized direction vector. Otherwise, it is the position of the light.
	 * @param lightColor color of the light, where (1,1,1) is white and (0,0,0) is black.
	 */
	Light(GraphicsEngine::LightType type, glm::vec3 lightData, glm::vec3 lightColor = glm::vec3(1));
};

}