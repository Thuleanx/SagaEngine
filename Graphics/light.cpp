#include "light.h"
#include "Engine/_Core/logger.h"
#include "glm/gtx/string_cast.hpp"

#include <iostream>
using namespace GraphicsEngine;


Light::Light(glm::vec3 direction, glm::vec3 lightColor) :
    m_lightType(DIRECTIONAL),
    m_lightDir(direction), 
    m_lightColor(lightColor) {}

Light::Light(glm::vec3 position, glm::vec3 attenuationFunction, glm::vec3 lightColor) :
    m_lightType(POINT),
    m_lightPos(position),
    m_lightFunction(attenuationFunction),
    m_lightColor(lightColor) {}

Light::Light(glm::vec3 position, glm::vec3 direction, glm::vec3 attenuationFunction, float innerAngle, float outerAngle, glm::vec3 lightColor) :
    m_lightType(SPOT),
    m_lightPos(position),
    m_lightDir(direction), 
    m_lightFunction(attenuationFunction), 
    m_lightAngle(innerAngle, outerAngle),
    m_lightColor(lightColor) {

    SDEBUG(glm::to_string(m_lightPos).c_str());
    SDEBUG(glm::to_string(m_lightDir).c_str());
    SDEBUG(glm::to_string(m_lightFunction).c_str());
    SDEBUG(glm::to_string(m_lightAngle).c_str());
}

Light::~Light(){
    
}

void Light::setPos(glm::vec3 newPos){
    m_lightPos = newPos;
}

glm::vec3 Light::getPos(){
    return m_lightPos;
}

void Light::translate(glm::vec3 delta){
    m_lightPos += delta;
}

void Light::setAttenuation(glm::vec3 attenuation){
    m_lightFunction = attenuation;
}

glm::vec3 Light::getAttenuation(){
    return m_lightFunction;
}

void Light::setDir(glm::vec3 newDir){
    m_lightDir = newDir;
}

glm::vec3 Light::getDir(){
    return m_lightDir;
}

void Light::rotate(float angle, glm::vec3 axis){
    m_lightDir = glm::vec3(glm::rotate(glm::mat4(1), angle, axis)*glm::vec4(m_lightDir, 0));
}

void Light::setColor(glm::vec3 newColor){
    m_lightColor = newColor;
}

glm::vec3 Light::getColor(){
    return m_lightColor;
}

void Light::setType(LightType newType){
    m_lightType = newType;
}

LightType Light::getType(){
    return m_lightType;
}

void Light::setAngle(float innerAngle, float outerAngle) {
    m_lightAngle = glm::vec2(innerAngle, outerAngle);
}

glm::vec2 Light::getAngle() { return m_lightAngle; }
