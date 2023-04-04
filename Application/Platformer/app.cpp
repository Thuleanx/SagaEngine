#include "app.h"
#include "Engine/Constants/colorthemes.h"
#include "Systems/playerControllerSystem.h"
#include "../General/Systems/playerInputSystem.h"
#include "../General/Systems/thirdPersonCameraSystem.h"
#include "Components/playerController.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "config.h"

namespace Platformer {
	App::App() {
		Saga::AudioEngine::loadBank(FMODSettings::bankName);
		Saga::AudioEngine::loadBank(FMODSettings::stringBankName);
		setupWorld();
	}
	App::~App() {
		Saga::AudioEngine::unloadBank(FMODSettings::bankName);
		Saga::AudioEngine::unloadBank(FMODSettings::stringBankName);
	}

	void App::setupWorld() {
		world = createGameWorld();
		setupSystems();

		auto setupCamera = [this](Saga::Entity player) {
			Saga::Entity camera = world->createEntity();
			std::shared_ptr<GraphicsEngine::Camera> defaultCamera 
				= std::make_shared<GraphicsEngine::Camera>();
			Saga::Camera& camComponent = *world->emplace<Saga::Camera>(camera, Saga::Camera{
				defaultCamera,  // camera
				true, // clearColorBufferOnDraw
				Saga::Theme_Nostalgic::colors[3], // clearColor
				glm::vec3(0.2, 1, 1), // coefficients for ambient, diffuse, and specular
				"phong", //shader
			});

			camComponent.camera->setPos(glm::vec3(0,8,0));

			world->emplace<Saga::Transform>(camera);
			Application::ThirdPersonCamera &tpcamera = *world->emplace<Application::ThirdPersonCamera>(camera,
				world->getComponent<Saga::Transform>(player));

			return camera;
		};

		auto setupPlane = [this]() {
			Saga::Entity plane = world->createEntity();
			Saga::Mesh& planeMesh = *world->emplace<Saga::Mesh>(plane, "Blender/noisyPlane.obj");
			Saga::Material& mat = *world->emplace<Saga::Material>(plane,
				Saga::Theme_Nostalgic::colors[1]);
			world->emplace<Saga::Transform>(plane);
			world->emplace<Saga::Collider>(plane);
			world->emplace<Saga::MeshCollider>(plane);

			return plane;
		};

		auto setupLights = [this]() {
			Saga::Entity lightEnt = world->createEntity();
			Saga::Light& light = *world->emplace<Saga::Light>(lightEnt, GraphicsEngine::LightType::DIRECTIONAL, glm::normalize(glm::vec3(-1, -1, 0)));
			Saga::Transform& transform = *world->emplace<Saga::Transform>(lightEnt);
			return lightEnt;
		};

		auto setupPlayer = [this]() {
			Saga::Entity player = world->createEntity();
			world->emplace<Application::PlayerInput>(player);
			world->emplace<Platformer::PlayerController>(player, 3);
			world->emplace<Saga::Material>(player, Saga::Theme_Nostalgic::colors[0]);
			world->emplace<Saga::Mesh>(player, Saga::Mesh::StandardType::Sphere);
			world->emplace<Saga::Collider>(player);
			world->emplace<Saga::EllipsoidCollider>(player, glm::vec3(0.5f)); // sphere collider
			world->emplace<Saga::RigidBody>(player);

			Saga::Transform* transform = world->emplace<Saga::Transform>(player);
			transform->transform->setPos(glm::vec3(0,2,0));
			// SDEBUG("Transform: <%f, %f, %f>", pos.x, pos.y, pos.z);
			return player;
		};

		auto setupBackingTrack = [this]() {
			Saga::Entity backingTrack = world->createEntity();
            world->emplace<Saga::AudioEmitter>(backingTrack, "event:/Loop", true);
			world->emplace<Saga::Transform>(backingTrack);
			return backingTrack;
		};

		Saga::Entity plane = setupPlane();
		Saga::Entity light = setupLights();
		Saga::Entity player = setupPlayer();
		Saga::Entity camera = setupCamera(player);
		Saga::Entity backingTrack = setupBackingTrack();
	}

	void App::setupSystems() {
		Saga::Systems::registerDrawSystem(world);
		Saga::Systems::registerCollisionSystem(world);
		Platformer::Systems::registerPlayerControllerSystem(world);
		Application::Systems::registerThirdPersonCameraSystem(world);

		Saga::Systems::setupAudioSystem(world);

		auto& systems = world->getSystems();

		// draw system
		systems.addStagedSystem(std::make_shared<Saga::System<>>(Saga::Systems::drawSystem), Saga::SystemManager::Stage::Draw);
		// draw on resize
		systems.addWindowResizeSystem(Saga::Systems::drawSystem_OnResize);

		// input reading
		systems.addKeyboardEventSystem(GLFW_KEY_W, Application::Systems::playerInputSystem_OnUpButton);
		systems.addKeyboardEventSystem(GLFW_KEY_A, Application::Systems::playerInputSystem_OnLeftButton);
		systems.addKeyboardEventSystem(GLFW_KEY_S, Application::Systems::playerInputSystem_OnDownButton);
		systems.addKeyboardEventSystem(GLFW_KEY_D, Application::Systems::playerInputSystem_OnRightButton);
		systems.addKeyboardEventSystem(GLFW_KEY_SPACE, Application::Systems::playerInputSystem_OnJumpButton);

		systems.addStagedSystem(std::make_shared<Saga::System<float, float>>(Application::Systems::thirdPersonCameraSystem), Saga::SystemManager::Stage::LateUpdate);
		systems.addMousePosSystem(Application::Systems::thirdPersonCameraSystem_OnMousePos);

		systems.addStagedSystem(std::make_shared<Saga::System<float, float>>(Platformer::Systems::playerControllerSystem));
	}
}
