#pragma once

#include "GLWrappers/shader.h"
#include "GLWrappers/vao.h"
#include "camera.h"
#include "light.h"
#include "material.h"
#include "modeltransform.h"
#include "shape.h"
#include "textrenderer.h"
#include <GL/glew.h>
#include <map>
#include <memory>
#include <vector>

namespace GraphicsEngine {
class Graphics {
public:
	Graphics();
	~Graphics();

	void initialize();
	void clearScreen(GLbitfield mask);
	void setClearColor(glm::vec3 clearColor);

	void setCameraData(std::shared_ptr<Camera> camera);

	void addShader(std::string shaderName, std::vector<GLenum> shaderTypes, std::vector<const char *> filepaths);
	void removeShader(std::string shaderName);
	void bindShader(std::string shaderName = "phong");
	std::shared_ptr<Shader> getShader(std::string shaderName);

	// Shape-related methods
	std::shared_ptr<Shape> addShape(std::string shapeName, std::vector<float> data, VAOAttrib attribs);
	std::vector<glm::vec3> addShape(std::string shapeName, std::string filepath);
	void removeShape(std::string shapeName);
	std::shared_ptr<Shape> getShape(std::string shapeName);
	void drawShape(std::shared_ptr<Shape> myShape, std::shared_ptr<ModelTransform> modelTransform, std::shared_ptr<Material> material = nullptr);
	void drawShape(std::shared_ptr<Shape> myShape, glm::mat4 modelMatrix, std::shared_ptr<Material> material = nullptr);

	// Obj-loading method
	std::vector<float> getObjData(std::string filepath);

	// Material-related methods
	std::shared_ptr<Material> addMaterial(std::string materialName, glm::vec3 color, float shininess = 1.f);
	std::shared_ptr<Material> addMaterial(std::string materialName, float shininess = 1.f);
	std::shared_ptr<Material> addMaterial(std::string materialName, std::string filePath, float shininess = 1.f);
	void removeMaterial(std::string materialName);
	std::shared_ptr<Material> getMaterial(std::string materialName);

	// Text and Font-related methods
	std::shared_ptr<Font> addFont(std::string fontName, std::string filepath);
	void removeFont(std::string fontName);
	std::shared_ptr<Font> getFont(std::string fontName);
	void drawUIText(std::shared_ptr<Font> font, std::string text, glm::vec2 anchorPosition, AnchorPoint anchorPoint, float textBoxWidth, float fontSize, float lineSpacing, glm::vec3 textColor);

	void setGlobalData(glm::vec3 globalCoeffs);

	void setLights(std::vector<std::shared_ptr<Light>> lights);
	void clearLights();

	void initializeGLEW();

	void setWindowSize(glm::ivec2 windowSize);
	glm::ivec2 getWindowSize();

	void setFramebufferSize(glm::ivec2 framebufferSize);
	glm::ivec2 getFramebufferSize();

private:
	glm::ivec2 m_windowSize;
	glm::ivec2 m_framebufferSize;

	std::shared_ptr<TextRenderer> m_textRenderer;

	std::map<std::string, std::shared_ptr<Shader>> m_shaders;
	std::map<std::string, std::shared_ptr<Shape>> m_shapes;
	std::map<std::string, std::shared_ptr<Material>> m_materials;
	std::map<std::string, std::shared_ptr<Font>> m_fonts;

	std::shared_ptr<Shader> m_active_shader;
};
} // namespace GraphicsEngine