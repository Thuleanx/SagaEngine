#pragma once

#include "GLWrappers/texture.h"
#include <memory>

#include "glm/glm.hpp"

namespace GraphicsEngine {
enum ColorSource {
    SOLID_COLOR,
    TEXTURE_COLOR,
    PER_VERTEX_COLOR
};

class Material {
public:
    Material(glm::vec3 color, float shininess = 1.f);                  // Constructor for setting SOLID_COLOR based material
    Material(float shininess = 1.f);                                   // Constructor for setting PER_VERTEX_COLOR based material
    Material(std::shared_ptr<Texture> texture, float shininess = 1.f); // Constructor for setting TEXTURE_COLOR based material
    ~Material();

    // Functions for setting uniforms
    ColorSource getColorSource();
    void setColorSource(ColorSource source);
    std::shared_ptr<Texture> getTexture();
    void setTexture(std::shared_ptr<Texture> texture);
    glm::vec3 getColor();
    glm::vec3 getEmission();
    void setColor(glm::vec3 color);
    float getShininess();
    void setShininess(float shininess);
    void setEmission(glm::vec3 color);

	void setTiling(glm::vec2 tiling);
	glm::vec2 getTiling();

private:
    ColorSource m_colorSource;
    std::shared_ptr<Texture> m_texture;
    glm::vec3 m_color;
    glm::vec3 m_emission = glm::vec3(0,0,0);
    float m_shininess;
	glm::vec2 m_tiling = glm::vec2(1,1);
};
} // namespace GraphicsEngine
