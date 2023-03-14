#pragma once
#include "Graphics/shape.h"
#include <glm/vec3.hpp>
#include <string>

namespace Saga {
/**
 * @brief A wrapper around a shared_ptr to a GraphicsEngine's shape.
 * @ingroup component
 */
struct Mesh {
	// these constants need to be in this order, due to how initializing member variables work (since vertexRange depends on attributes)
	GraphicsEngine::VAOAttrib attributes; //!< determines what data holds, whether it's position, normal, uv, or color, or any combination of them.
	int vertexRange; //!< how many floats does it take to store 1 vertex. Varies depending on the attributes chosen. Should be equal to 3*(?pos + ?norm + ?color) + 2(?uv).

	/**
	 * @brief Different types of meshes loaded into the graphics engine. 
	 *
	 */
	enum class StandardType {
		Quad,
		Cone,
		Cube,
		Sphere,
		Cylinder
	};

	std::shared_ptr<GraphicsEngine::Shape> mesh;
	std::vector<float> data; //!< packed vertex data 

	operator const std::shared_ptr<GraphicsEngine::Shape> &() const { return mesh; }

	/**
	 * @brief Construct a new Mesh object
	 * 
	 */
	Mesh();

	/**
	 * @brief Construct a new Mesh object using a standard mesh type. 
	 * These are default objects in the engine. 
	 * @see StandardType
	 * 
	 * @param type the type of the shape of the mesh.
	 */
	Mesh(StandardType type);

	/**
	 * @brief Construct a new Mesh object loaded from the specific filename. 
	 * Does not support instancing out of the gate.
	 * 
	 * @param filepath the filepath of the mesh to load. 
	 * 	This must be an .obj file format, and the path is relative to the root of the project.
	 */
	Mesh(const std::string &filepath);

	/**
	 * @brief Construct a new Mesh object using the mesh data.
	 * 
	 * @param data coordinates of the mesh, in POS | NORMAL form.
	 */
	Mesh(std::vector<float> data, GraphicsEngine::VAOAttrib attributes);

	glm::vec3 getPos(int index);
	int getTrianglesCnt();

	/**
	 * @brief Used for assigning new shape name in the graphics engine.
	 * @return uint64_t& a new, unique integer.
	 */
	static auto id_value()
		-> uint64_t & {
		static uint64_t the_id;
		return the_id;
	}

	/**
	 * @brief Get the number of floats needed to store a vertex.
	 * 
	 * @param attributes the attributes we need to store for a single vertex.
	 * @return int number of floats used to store a vertex. Generally, position costs 3 floats, so does color and normal. uv coordinates costs two to store.
	 */
	static int getVertexRange(GraphicsEngine::VAOAttrib attributes);
};
} // namespace Saga
