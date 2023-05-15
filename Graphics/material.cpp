#include "material.h"
using namespace GraphicsEngine;

Material::Material(glm::vec3 color, float shininess):
    m_colorSource(ColorSource::SOLID_COLOR),
    m_color(color),
    m_shininess(shininess)
{

}

Material::Material(float shininess):
    m_colorSource(ColorSource::PER_VERTEX_COLOR),
    m_shininess(shininess)
{

}

Material::Material(std::shared_ptr<Texture> texture, float shininess):
    m_colorSource(ColorSource::TEXTURE_COLOR),
    m_texture(texture),
    m_shininess(shininess)
{

}

Material::~Material(){

}

ColorSource Material::getColorSource(){
    return m_colorSource;
}

void Material::setColorSource(ColorSource source){
    m_colorSource = source;
}

std::shared_ptr<Texture> Material::getTexture(){
    return m_texture;
}

void Material::setTexture(std::shared_ptr<Texture> texture){
    m_texture = texture;
}

glm::vec3 Material::getColor(){
    return m_color;
}

glm::vec3 Material::getEmission(){
    return m_emission;
}

void Material::setColor(glm::vec3 color){
    m_color = color;
}

float Material::getShininess(){
    return m_shininess;
}

void Material::setShininess(float shininess){
    m_shininess = shininess;
}

void Material::setEmission(glm::vec3 color) {
    m_emission = color;
}

void Material::setTiling(glm::vec2 tiling){
	m_tiling = tiling;
}

glm::vec2 Material::getTiling() {
	return m_tiling;
}
