#pragma once

#include "glm/ext.hpp"
#include "glm/glm.hpp"

namespace GraphicsEngine {
class ModelTransform {
public:
    ModelTransform();
    ~ModelTransform();

    ModelTransform* setPos(glm::vec3 pos);
    ModelTransform* translate(glm::vec3 delta);
    glm::vec3 getPos();
    ModelTransform* setScale(glm::vec3 scale);
    ModelTransform* setScale(float scale);
    ModelTransform* scale(glm::vec3 scale);
    ModelTransform* scale(float scale);
    glm::vec3 getScale();
    ModelTransform* setRotation(float angle, glm::vec3 axis);
    ModelTransform* setRotation(glm::mat4 r);
    ModelTransform* rotate(float angle, glm::vec3 axis);
    ModelTransform* rotate(glm::mat4 r);
    glm::mat4 getRotation();
    glm::mat4 getModelMatrix();
    void clear();

private:
    glm::vec3 m_pos;
    glm::vec3 m_scale;
    glm::mat4 m_rotation = glm::mat4(1);
};
} // namespace GraphicsEngine
