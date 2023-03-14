#include "material.h"

namespace Saga {
/**
 * @brief Construct a new Material object.
 * @warning this will leave you with a pointer to a null material. Please assign it before use.
 */
Material::Material() { }

/**
 * @brief Constructor for setting SOLID_COLOR based material.
 *
 * @param color rgb color of the material. Each components are in the range [0,1].
 * @param shininess shininess of the material. Used for phong illumination.
 */
Material::Material(glm::vec3 color, float shininess) {
    material = GraphicsEngine::Global::graphics.addMaterial("@saga_mat::" + std::to_string(++id_value()), color, shininess);
}

/**
 * @brief Constructor for setting PER_VERTEX_COLOR based material.
 *
 * @param shininess shininess of the material. Used for phong illumination.
 */
Material::Material(float shininess) {
    material = GraphicsEngine::Global::graphics.addMaterial("@saga_mat::" + std::to_string(++id_value()), shininess);
}

/**
 * @brief Constructor for setting TEXTURE_COLOR based material
 * 
 * @param texture pointer to a texture object.
 * @param shininess shininess of the material. Used for phong illumination.
 */
Material::Material(std::shared_ptr<GraphicsEngine::Texture> texture, float shininess) {
    material = GraphicsEngine::Global::graphics.addMaterial("@saga_mat::" + std::to_string(++id_value()), shininess);
}

/**
 * @brief Used for assigning new material name in the graphics engine.
 * @return uint64_t& a new, unique integer.
 */
static auto id_value()
	-> uint64_t & {
	static uint64_t the_id;
	return the_id;
}
}
