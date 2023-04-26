#pragma once

#include "glm/ext.hpp"
#include "glm/glm.hpp"

namespace GraphicsEngine {
enum LightType {
    POINT,
    DIRECTIONAL,
    SPOT
};

class Light {
public:
    Light(glm::vec3 direction, glm::vec3 lightColor);
    Light(glm::vec3 position, glm::vec3 attenuationFunction, glm::vec3 lightColor);
    Light(glm::vec3 position, glm::vec3 direction, glm::vec3 attenuationFunction, float innerAngle, float outerAngle, glm::vec3 lightColor);
    ~Light();

    // Functions for point lights
    void setPos(glm::vec3 newPos);
    glm::vec3 getPos();
    void translate(glm::vec3 delta);
    void setAttenuation(glm::vec3 attenuation);
    glm::vec3 getAttenuation();

    // Functions for directional lights
    void setDir(glm::vec3 newDir);
    glm::vec3 getDir();
    void rotate(float angle, glm::vec3 axis);

    // Utility functions for light color
    void setColor(glm::vec3 newColor);
    glm::vec3 getColor();

    // Utility functions for the light type
    void setType(LightType newType);
    LightType getType();

    // Utility function for the light angle
    void setAngle(float innerAngle, float outerAngle);
    glm::vec2 getAngle();

private:
    LightType m_lightType;
    glm::vec3 m_lightPos;
    glm::vec3 m_lightDir;
    glm::vec3 m_lightColor;
    glm::vec3 m_lightFunction;
    glm::vec2 m_lightAngle;
};
} // namespace GraphicsEngine
