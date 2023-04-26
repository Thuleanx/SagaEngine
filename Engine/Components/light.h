#pragma once

#include <memory>
#include <glm/vec3.hpp>

namespace GraphicsEngine {
    class Light;
}

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
     * @brief Consturct a new Directional Light object.
     *
     * @param direction the direction of the light
     * @param lightColor the rgb color of the light, in the range [0,1] for each component.
     */
    Light(glm::vec3 direction, glm::vec3 lightColor);

    /**
     * @brief Construct a new Point Light object.
     *
     * @param position
     * @param attenuationFunction here, the falloff is defined as 1 / (atten[0] + atten[1] * d + atten[2]*d) where d is distance
     * to the light.
     * @param lightColor the rgb color of the light, in the range [0,1] for each component.
     */
    Light(glm::vec3 position, glm::vec3 attenuationFunction, glm::vec3 lightColor);

    /**
     * @brief Construct a new Spot Light object.
     *
     * @param position
     * @param direction
     * @param innerAngle the inner angle of the spot light, in radians.
     * @param outerAngle the outer angle of the spot light, in radians. Must be at least inner angle
     * @param lightColor the rgb color of the light, in the range [0,1] for each component.
     */
    Light(glm::vec3 position, glm::vec3 direction, glm::vec3 attenuationFunction, float innerAngle, float outerAngle, glm::vec3 lightColor = glm::vec3(1));
};

}
