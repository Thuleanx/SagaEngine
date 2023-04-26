#include "light.h"
#include "Graphics/light.h"

namespace Saga {
	Light::Light() {}

    Light::Light(glm::vec3 direction, glm::vec3 lightColor) {
        light = std::make_shared<GraphicsEngine::Light>(
            direction,
            lightColor
        );
    }

    Light::Light(glm::vec3 position, glm::vec3 attenuationFunction, glm::vec3 lightColor) {
        light = std::make_shared<GraphicsEngine::Light>(
            position,
            attenuationFunction,
            lightColor
        );
    }

    Light::Light(glm::vec3 position, glm::vec3 direction, glm::vec3 attenuationFunction, float innerAngle, float outerAngle, glm::vec3 lightColor) {
        light = std::make_shared<GraphicsEngine::Light>(
            position,
            direction,
            attenuationFunction,
            innerAngle,
            outerAngle,
            lightColor
        );
    }
}
