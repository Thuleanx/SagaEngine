#include "modeltransform.h"
#include <iostream>
using namespace GraphicsEngine;

ModelTransform::ModelTransform():
    m_pos(glm::vec3(0)),
    m_scale(glm::vec3(1)),
    m_rotation(glm::mat4(1))
{

}

ModelTransform::~ModelTransform(){

}

ModelTransform* ModelTransform::setPos(glm::vec3 pos){
    m_pos = pos;
    return this;
}

ModelTransform* ModelTransform::translate(glm::vec3 delta) {
    m_pos += delta;
    return this;
}

glm::vec3 ModelTransform::getPos() {
    return m_pos;
}

ModelTransform* ModelTransform::setScale(glm::vec3 scale) {
    m_scale = scale;
    return this;
}

ModelTransform* ModelTransform::setScale(float scale) {
    m_scale = glm::vec3(scale);
    return this;
}

ModelTransform* ModelTransform::scale(glm::vec3 scale) {
    m_scale *= scale;
    return this;
}

ModelTransform* ModelTransform::scale(float scale) {
    m_scale *= scale;
    return this;
}

glm::vec3 ModelTransform::getScale() {
    return m_scale;
}

ModelTransform* ModelTransform::setRotation(float angle, glm::vec3 axis) {
    m_rotation = glm::rotate(glm::mat4(1), angle, axis);
    return this;
}

ModelTransform* ModelTransform::setRotation(glm::mat4 r) {
    m_rotation = r;
    return this;
}

ModelTransform* ModelTransform::rotate(float angle, glm::vec3 axis) {
    m_rotation = glm::rotate(m_rotation, angle, axis);
    return this;
}

ModelTransform* ModelTransform::rotate(glm::mat4 r) {
    m_rotation = r * m_rotation;
    return this;
}

glm::mat4 ModelTransform::getRotation() {
    return m_rotation;
}

glm::mat4 ModelTransform::getModelMatrix() {
    glm::mat4 model = glm::mat4(1);
    model = glm::translate(model, m_pos);
    model = model * m_rotation;
    model = glm::scale(model, m_scale);
    return model;
}

void ModelTransform::clear(){
    m_pos = glm::vec3(0, 0, 0);
    m_scale = glm::vec3(1, 1, 1);
    m_rotation = glm::mat4(1);
}
