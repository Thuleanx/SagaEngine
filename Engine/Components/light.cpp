#include "light.h"

namespace Saga {
	Light::Light() {}

    Light::Light(GraphicsEngine::LightType type, glm::vec3 lightData, glm::vec3 lightColor) {
		light = std::make_shared<GraphicsEngine::Light>(type, lightData, lightColor);
	}
}
