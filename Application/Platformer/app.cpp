#include "app.h"
#include "Application/Platformer/Components/friendController.h"
#include "Application/Platformer/Systems/friendControllerSystem.h"
#include "Engine/Components/collider.h"
#include "Engine/Constants/colorthemes.h"
#include "Engine/Entity/entity.h"
#include "Systems/playerControllerSystem.h"
#include "../General/Systems/playerInputSystem.h"
#include "../General/Systems/thirdPersonCameraSystem.h"
#include "Components/playerController.h"
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "config.h"
#include <random>
#include <chrono>

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

        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

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
            Saga::Transform* transform = world->emplace<Saga::Transform>(plane);
            transform->transform->setScale(2);

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
			world->emplace<Platformer::PlayerController>(player, 5);
			world->emplace<Saga::Material>(player, Saga::Theme_Nostalgic::colors[0]);
			world->emplace<Saga::Mesh>(player, Saga::Mesh::StandardType::Cylinder);
			world->emplace<Saga::Collider>(player);
			world->emplace<Saga::EllipsoidCollider>(player, glm::vec3(0.5f)); // sphere collider
			world->emplace<Saga::RigidBody>(player);
            world->emplace<Saga::CylinderCollider>(player, 1, 0.5);

			Saga::Transform* transform = world->emplace<Saga::Transform>(player);
			transform->transform->setPos(glm::vec3(0,5,0));
			return player;
		};

        auto setupFriend = [this, &rng]() {
            Saga::Entity fr = world->createEntity();
            float movespeed = std::uniform_real_distribution<float>(4.f, 9.f)(rng);
            float orbitDistance = std::uniform_real_distribution<float>(2,3)(rng);
            int colorIndex = std::uniform_int_distribution<int>(0,8)(rng);

            world->emplace<Platformer::FriendController>(fr, 
                movespeed, orbitDistance
            );
			world->emplace<Saga::Material>(fr, Saga::Theme_Nostalgic::colors[colorIndex]);
			world->emplace<Saga::Mesh>(fr, Saga::Mesh::StandardType::Cylinder);
			world->emplace<Saga::Collider>(fr);
			world->emplace<Saga::EllipsoidCollider>(fr, glm::vec3(0.5f)); // sphere collider
			world->emplace<Saga::RigidBody>(fr);
            world->emplace<Saga::CylinderCollider>(fr, 1, 0.5);

            float randomRadiant = std::uniform_real_distribution<float>(0.0, 2*M_PI)(rng);

            glm::vec3 spawnPos = glm::vec3(sin(randomRadiant),0,cos(randomRadiant)) * orbitDistance;
            spawnPos.y = 5;

			Saga::Transform* transform = world->emplace<Saga::Transform>(fr);
			transform->transform->setPos(spawnPos);
			return fr;

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

        int friendCnt = 10;
        while (friendCnt --> 0) 
            setupFriend();
	}

	void App::setupSystems() {
		Saga::Systems::registerDrawSystem(world);
		Saga::Systems::registerCollisionSystem(world);
		Platformer::Systems::registerPlayerControllerSystem(world);
		Platformer::Systems::registerFriendControllerSystem(world);
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
		systems.addStagedSystem(std::make_shared<Saga::System<float, float>>(Platformer::Systems::friendControllerSystem));
	}
}
