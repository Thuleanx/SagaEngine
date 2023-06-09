#include "shader.h"
#include "glm/gtx/string_cast.hpp"

using namespace GraphicsEngine;

Shader::Shader(std::vector<GLenum> shaderTypes, std::vector<const char *> filepaths) {
	m_handle = ShaderLoader::createShaderProgram(shaderTypes, filepaths);
}

Shader::~Shader() {
	glDeleteProgram(m_handle);
}

void Shader::bind() {
	glUseProgram(m_handle);
}

void Shader::unbind() {
	glUseProgram(0);
}

GLuint Shader::getHandle() {
	return m_handle;
}

void Shader::setMaterial(std::shared_ptr<Material> material) {
	ColorSource color_source = material->getColorSource();
	switch (color_source) {
	case ColorSource::SOLID_COLOR:
		glUniform1i(glGetUniformLocation(m_handle, "colorSource"), 0);
		glUniform3f(glGetUniformLocation(m_handle, "objColor"), material->getColor().r, material->getColor().g, material->getColor().b);
		break;
	case ColorSource::TEXTURE_COLOR:
		glUniform1i(glGetUniformLocation(m_handle, "colorSource"), 1);
		material->getTexture()->bind();
		glUniform1i(glGetUniformLocation(m_handle, "objTexture"), material->getTexture()->getTexUnitUint());
		break;
	case ColorSource::PER_VERTEX_COLOR:
		glUniform1i(glGetUniformLocation(m_handle, "colorSource"), 2);
		break;
	}
	float shininess = material->getShininess();
	glUniform1f(glGetUniformLocation(m_handle, "shininess"), shininess);
	glUniform3f(glGetUniformLocation(m_handle, "emission"), material->getEmission().r, material->getEmission().g, material->getEmission().b);
}

void Shader::setCamera(std::shared_ptr<Camera> camera) {
	glUniformMatrix4fv(glGetUniformLocation(m_handle, "view"), 1, GL_FALSE, glm::value_ptr(camera->getView()[0]));
	glUniformMatrix4fv(glGetUniformLocation(m_handle, "projection"), 1, GL_FALSE, glm::value_ptr(camera->getProjection()[0]));
	glUniform3f(glGetUniformLocation(m_handle, "worldSpace_camPos"), camera->getPos().x, camera->getPos().y, camera->getPos().z);

    setFloat("cameraNearFarDistance", camera->getNearFarDistance());
}

void Shader::setModelTransform(std::shared_ptr<ModelTransform> modelTransform) {
	glUniformMatrix4fv(glGetUniformLocation(m_handle, "model"), 1, GL_FALSE, glm::value_ptr(modelTransform->getModelMatrix()[0]));
}

void Shader::setModelTransform(glm::mat4 modelMatrix) {
	glUniformMatrix4fv(glGetUniformLocation(m_handle, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

void Shader::setGlobalCoeffs(glm::vec3 coeffs, glm::vec3 ambientColor) {
	glUniform3f(glGetUniformLocation(m_handle, "coeffs"), coeffs.x, coeffs.y, coeffs.z);
	glUniform3f(glGetUniformLocation(m_handle, "ambientColor"), ambientColor.r, ambientColor.g, ambientColor.b);
}

void Shader::setLights(std::vector<std::shared_ptr<Light>> lights) {
	if (lights.size()) {
		int numLights = std::min(int(lights.size()), 16);
		std::vector<int> lightType;
		std::vector<float> lightColor;
		std::vector<float> lightFunction;
        std::vector<float> lightAngle;
		std::vector<float> worldSpace_lightPos;
		std::vector<float> worldSpace_lightDir;

		lightType.resize(numLights);
		lightColor.resize(numLights * 3);
		lightFunction.resize(numLights * 3);
        lightAngle.resize(numLights*2);
		worldSpace_lightPos.resize(numLights * 3);
		worldSpace_lightDir.resize(numLights * 3);
		for (int i = 0; i < numLights; i++) {
			lightColor[i * 3] = lights[i]->getColor().r;
			lightColor[i * 3 + 1] = lights[i]->getColor().g;
			lightColor[i * 3 + 2] = lights[i]->getColor().b;
			glm::vec3 camLightData;
			switch (lights[i]->getType()) {
			case LightType::POINT:
				lightType[i] = 0;
				lightFunction[i * 3] = lights[i]->getAttenuation().x;
				lightFunction[i * 3 + 1] = lights[i]->getAttenuation().y;
				lightFunction[i * 3 + 2] = lights[i]->getAttenuation().z;
				worldSpace_lightPos[i * 3] = lights[i]->getPos().x;
				worldSpace_lightPos[i * 3 + 1] = lights[i]->getPos().y;
				worldSpace_lightPos[i * 3 + 2] = lights[i]->getPos().z;
				break;
			case LightType::DIRECTIONAL:
				lightType[i] = 1;
				worldSpace_lightDir[i * 3] = lights[i]->getDir().x;
				worldSpace_lightDir[i * 3 + 1] = lights[i]->getDir().y;
				worldSpace_lightDir[i * 3 + 2] = lights[i]->getDir().z;
				break;
            case LightType::SPOT:
                lightType[i] = 2;

                SDEBUG(glm::to_string(lights[i]->getPos()).c_str());
                SDEBUG(glm::to_string(lights[i]->getDir()).c_str());

				worldSpace_lightPos[i * 3] = lights[i]->getPos().x;
				worldSpace_lightPos[i * 3 + 1] = lights[i]->getPos().y;
				worldSpace_lightPos[i * 3 + 2] = lights[i]->getPos().z;

				worldSpace_lightDir[i * 3] = lights[i]->getDir().x;
				worldSpace_lightDir[i * 3 + 1] = lights[i]->getDir().y;
				worldSpace_lightDir[i * 3 + 2] = lights[i]->getDir().z;

				lightFunction[i * 3] = lights[i]->getAttenuation().x;
				lightFunction[i * 3 + 1] = lights[i]->getAttenuation().y;
				lightFunction[i * 3 + 2] = lights[i]->getAttenuation().z;

                lightAngle[i*2] = lights[i]->getAngle().x;
                lightAngle[i*2+1] = lights[i]->getAngle().y;

                break;
			}
		}
		glUniform1i(glGetUniformLocation(m_handle, "numLights"), numLights);
		Debug::checkGLError();
		glUniform1iv(glGetUniformLocation(m_handle, "lightType"), numLights, lightType.data());
		Debug::checkGLError();
		glUniform3fv(glGetUniformLocation(m_handle, "lightColor"), numLights, lightColor.data());
		Debug::checkGLError();
		glUniform3fv(glGetUniformLocation(m_handle, "lightFunction"), numLights, lightFunction.data());
		Debug::checkGLError();

		glUniform2fv(glGetUniformLocation(m_handle, "lightAngle"), numLights, lightAngle.data());
		Debug::checkGLError();

		glUniform3fv(glGetUniformLocation(m_handle, "worldSpace_lightPos"), numLights, worldSpace_lightPos.data());
		Debug::checkGLError();
		// std::cout << worldSpace_lightDir[0] << std::endl;
		glUniform3fv(glGetUniformLocation(m_handle, "worldSpace_lightDir"), numLights, worldSpace_lightDir.data());
		Debug::checkGLError();
	}
}

void Shader::clearLights() {
	glUniform1i(glGetUniformLocation(m_handle, "numLights"), 0);
}

void Shader::setTextUniforms(float screenWidth, float screenHeight, glm::vec3 color) {
	glm::mat4 projection = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);
	glUniformMatrix4fv(glGetUniformLocation(m_handle, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(glGetUniformLocation(m_handle, "textColor"), color.r, color.g, color.b);
	Debug::checkGLError();
}

void Shader::setMat4(const std::string &name, glm::mat4 value) {
	glUniformMatrix4fv(glGetUniformLocation(m_handle, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	Debug::checkGLError();
}

void Shader::setVec3(const std::string &name, glm::vec3 value) {
	glUniform3f(glGetUniformLocation(m_handle, name.c_str()), value.r, value.g, value.b);
	Debug::checkGLError();
}

void Shader::setVec4(const std::string &name, glm::vec4 value) {
	glUniform4f(glGetUniformLocation(m_handle, name.c_str()), value.r, value.g, value.b, value.a);
	Debug::checkGLError();
}

void Shader::setVec2(const std::string &name, glm::vec2 value) {
	glUniform2f(glGetUniformLocation(m_handle, name.c_str()), value.r, value.g);
	Debug::checkGLError();
}

void Shader::setFloat(const std::string &name, float value) {
	glUniform1f(glGetUniformLocation(m_handle, name.c_str()), value);
	Debug::checkGLError();
}

void Shader::setSampler(const std::string &name, int value) {
	glUniform1i(glGetUniformLocation(m_handle, name.c_str()), value);
	Debug::checkGLError();
}

void Shader::setInt(const std::string &name, int value) {
	glUniform1i(glGetUniformLocation(m_handle, name.c_str()), value);
	Debug::checkGLError();
}
