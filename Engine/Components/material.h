#pragma once

#include "Graphics/global.h"
#include "Graphics/material.h"
#include <memory>

namespace Saga {

/**
 * @brief Wrapper around the GraphisEngine's material.
 * @ingroup component
 */
struct Material {
	std::shared_ptr<GraphicsEngine::Material> material;

	operator std::shared_ptr<GraphicsEngine::Material> &() { return material; }
	operator const std::shared_ptr<GraphicsEngine::Material> &() const { return material; }

	/**
	 * @brief Construct a new Material object.
	 * @warning this will leave you with a pointer to a null material. Please assign it before use.
	 */
	Material();

	/**
	 * @brief Constructor for setting SOLID_COLOR based material.
	 *
	 * @param color rgb color of the material. Each components are in the range [0,1].
	 * @param shininess shininess of the material. Used for phong illumination.
	 */
	Material(glm::vec3 color, float shininess = 1.f);

	/**
	 * @brief Constructor for setting PER_VERTEX_COLOR based material.
	 *
	 * @param shininess shininess of the material. Used for phong illumination.
	 */
	Material(float shininess);

	/**
	 * @brief Constructor for setting TEXTURE_COLOR based material
	 * 
	 * @param texture pointer to a texture object.
	 * @param shininess shininess of the material. Used for phong illumination.
	 */
	Material(std::shared_ptr<GraphicsEngine::Texture> texture, float shininess = 1.f); 

	/**
	 * @brief Used for assigning new material name in the graphics engine.
	 * @return uint64_t& a new, unique integer.
	 */
	static auto id_value()
		-> uint64_t & {
		static uint64_t the_id;
		return the_id;
	}
};

} // namespace Saga