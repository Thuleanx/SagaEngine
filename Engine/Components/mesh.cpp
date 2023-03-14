#include "mesh.h"
#include <string>
#include "Graphics/global.h"
#include "Graphics/shapedata.h"
#include <stdexcept>
#include "../_Core/logger.h"
#include "../_Core/asserts.h"

namespace Saga {
	
using attr = GraphicsEngine::VAOAttrib;

Mesh::Mesh() : attributes(), vertexRange(0) {}

Mesh::Mesh(StandardType type) : attributes(attr::POS | attr::NORM | attr::UV), vertexRange(getVertexRange(attributes)) {
	std::string shapeName = "";
	if (type == StandardType::Quad) 		shapeName = "quad";
	if (type == StandardType::Cylinder) 	shapeName = "cylinder";
	if (type == StandardType::Cone) 		shapeName = "cone";
	if (type == StandardType::Sphere) 		shapeName = "sphere";
	if (type == StandardType::Cube) 		shapeName = "cube";

	mesh = GraphicsEngine::Global::graphics.getShape(shapeName);

	if (type == StandardType::Quad) 		data = GraphicsEngine::quadVertexBufferData;
	if (type == StandardType::Cylinder) 	data = GraphicsEngine::cylinderVertexBufferData;
	if (type == StandardType::Cone) 		data = GraphicsEngine::coneVertexBufferData;
	if (type == StandardType::Sphere) 		data = GraphicsEngine::sphereVertexBufferData;	
	if (type == StandardType::Cube) 		data = GraphicsEngine::cubeVertexBufferData;

	if (data.size() % vertexRange) 
		SERROR("Loaded a mesh of standard type %d but the data size (%d) is not divisible by number of floats (%d) required to store a vertex.", type, data.size(), vertexRange);
}

Mesh::Mesh(const std::string &filepath) : attributes(attr::POS | attr::NORM), vertexRange(getVertexRange(attributes)) {
	data = GraphicsEngine::Global::graphics.getObjData(filepath);
    mesh = GraphicsEngine::Global::graphics.addShape("@saga_shape::" + std::to_string(++id_value()), data, attributes);

	if (data.size() % vertexRange) 
        SERROR("Loaded a mesh from file '%s' but the data size (%d) is not divisible by number of floats (%d) required to store a vertex.", filepath.c_str(), data.size(), vertexRange);
}

Mesh::Mesh(std::vector<float> data, GraphicsEngine::VAOAttrib attributes) : data(data), attributes(attributes), vertexRange(getVertexRange(attributes)) {
    mesh = GraphicsEngine::Global::graphics.addShape("@saga_shape::" + std::to_string(++id_value()), data, attributes);

	if (data.size() % vertexRange) 
		SERROR("Loaded a mesh from data but the data size (%d) is not divisible by number of floats (%d) required to store a vertex.", data.size(), vertexRange);
}

glm::vec3 Mesh::getPos(int index) {
	SASSERT_MESSAGE(index >= 0 && index * vertexRange < data.size(), "Index cannot be below 0 or above tri count.");
	glm::vec3 pos(data[index*vertexRange + 0], data[index*vertexRange + 1], data[index*vertexRange + 2]);
	return pos;
}

int Mesh::getTrianglesCnt() {
	return data.size() / vertexRange / 3;
}

int Mesh::getVertexRange(GraphicsEngine::VAOAttrib attributes) {
	return (attributes & GraphicsEngine::VAOAttrib::POS) * 3 +
		(attributes & GraphicsEngine::VAOAttrib::NORM) * 3 +
		(attributes & GraphicsEngine::VAOAttrib::UV) * 2 +
		(attributes & GraphicsEngine::VAOAttrib::COLOR) * 3;
}

}
