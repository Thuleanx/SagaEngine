#pragma once

#include "GL/glew.h"
#include <vector>

namespace GraphicsEngine {

//Quad, Cube, Sphere, Cylinder, and Cone data with pos, norm, uvs
extern std::vector<GLfloat> quadVertexBufferData;
extern std::vector<GLfloat> cubeVertexBufferData;
extern std::vector<GLfloat> sphereVertexBufferData;
extern std::vector<GLfloat> cylinderVertexBufferData;
extern std::vector<GLfloat> coneVertexBufferData;

}