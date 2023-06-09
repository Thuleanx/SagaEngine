#pragma once

#include "../camera.h"
#include "../light.h"
#include "../material.h"
#include "../modeltransform.h"
#include "../shaderloader.h"

namespace GraphicsEngine {
class Shader {
public:
	Shader(std::vector<GLenum> shaderTypes, std::vector<const char *> filepaths);
	~Shader();

	void bind();
	void unbind();
	GLuint getHandle();
	void setMaterial(std::shared_ptr<Material> material);
	void setCamera(std::shared_ptr<Camera> camera);
	void setModelTransform(std::shared_ptr<ModelTransform> modelTransform);
	void setModelTransform(glm::mat4 modelMatrix);
	void setGlobalCoeffs(glm::vec3 coeffs, glm::vec3 ambientColor = glm::vec3(1,1,1));
	void setLights(std::vector<std::shared_ptr<Light>> lights);
	void clearLights();
	void setTextUniforms(float screenWidth, float screenHeight, glm::vec3 color);

    // generic setters
    void setMat4(const std::string &name, glm::mat4 value);
    void setVec4(const std::string &name, glm::vec4 value);
    void setVec3(const std::string &name, glm::vec3 value);
    void setVec2(const std::string &name, glm::vec2 value);
    void setFloat(const std::string &name, float value);
    void setSampler(const std::string &name, int value);
    void setInt(const std::string &name, int value);

private:
	GLuint m_handle;
};
} // namespace GraphicsEngine
