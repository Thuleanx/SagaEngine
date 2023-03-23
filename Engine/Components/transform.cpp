#include "transform.h"

namespace Saga {

Transform::Transform() { 
	transform = std::make_shared<GraphicsEngine::ModelTransform>(); 
}

glm::vec3 Transform::getPos() const {
	return transform->getPos();
}

glm::vec3 Transform::getUp() const {
	glm::mat4 matrix = transform->getRotation();
	// should be in column 1 (0-indexed)
	return glm::vec3(matrix[1][0], matrix[1][1], matrix[1][2]);
}

glm::vec3 Transform::getForward() const {
	glm::mat4 matrix = transform->getRotation();
	// should be in column 2 (0-indexed)
	return glm::vec3(matrix[2][0], matrix[2][1], matrix[2][2]);
}

} // namespace Saga