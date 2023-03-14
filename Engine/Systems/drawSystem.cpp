#include "drawSystem.h"
#include "Graphics/global.h"
#include "../Gameworld/gameworld.h"
#include "../Components/transform.h"
#include "../Components/mesh.h"
#include "../Components/camera.h"
#include "../Components/material.h"
#include "../Components/light.h"
#include "../_Core/asserts.h"
#include <iostream>

namespace Saga::Systems {

void drawSystem(std::shared_ptr<Saga::GameWorld> world) {
	for (Saga::Camera& camera : *world->viewAll<Camera>()) {
		using namespace GraphicsEngine::Global;
		// graphics setup 

		// clear color buffer
		if (camera.clearColorBufferOnDraw) {
			graphics.setClearColor(camera.clearColor);
			graphics.clearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		} else graphics.clearScreen(GL_DEPTH_BUFFER_BIT);

		// set shader
		SASSERT_MESSAGE(camera.shader != "", "Camera cannot have an empty shader");
		graphics.bindShader(camera.shader);

		std::vector<std::shared_ptr<GraphicsEngine::Light>> lights;
		for (auto &[entity, light, transform] : *world->viewGroup<Light, Transform>()) {
			SASSERT_MESSAGE(light->light, "Light reference cannot be null.");
			SASSERT_MESSAGE(transform->transform, "Transform reference cannot be null.");

			light->light->setPos(transform->transform->getPos());
			lights.push_back(light->light);
		}
		graphics.setLights(lights);
		graphics.setGlobalData(camera.globalShaderCoefs);
		graphics.setCameraData(camera);

		// loop through and draw all entities
		for (auto &[entity, material, mesh, transform] : *world->viewGroup<Material, Mesh, Transform>()) {
			SASSERT_MESSAGE(material->material, "Material cannot be null.");
			SASSERT_MESSAGE(mesh->mesh, "Mesh shape cannot be null.");
			SASSERT_MESSAGE(transform->transform, "Transform cannot be null.");

			graphics.drawShape(*mesh, *transform, *material);
		}
	}
}

void drawSystem_OnResize(std::shared_ptr<GameWorld> world, int width, int height) {
	for (Saga::Camera& camera : *world->viewAll<Camera>()) 
		camera.camera->resize(width, height);
}

void registerDrawSystem(std::shared_ptr<GameWorld> world) {
	world->registerGroup<Material, Mesh, Transform>();
	world->registerGroup<Light, Transform>();
}

} // namespace Saga::System
