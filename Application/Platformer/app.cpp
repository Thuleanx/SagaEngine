#include "app.h"
#include "Application/Platformer/Behaviours/computePathTowards.h"
#include "Application/Platformer/Behaviours/loadPlayerPosition.h"
#include "Application/Platformer/Behaviours/walkPath.h"
#include "Application/Platformer/Components/friendController.h"
#include "Application/Platformer/Components/simpleTestAI.h"
#include "Application/Platformer/Systems/friendControllerSystem.h"
#include "Application/Platformer/Systems/simpleTestAISystem.h"
#include "Engine/Components/BehaviourTree/behaviourtree.h"
#include "Engine/Components/BehaviourTree/blackboard.h"
#include "Engine/Components/BehaviourTree/compositenodes.h"
#include "Engine/Components/BehaviourTree/tasknodes.h"
#include "Engine/Components/collider.h"
#include "Engine/Components/navigation/navMeshData.h"
#include "Engine/Constants/colorthemes.h"
#include "Engine/Entity/entity.h"
#include "Engine/Systems/aiSystem.h"
#include "Systems/playerControllerSystem.h"
#include "../General/Systems/playerInputSystem.h"
#include "../General/Systems/thirdPersonCameraSystem.h"
#include "Components/playerController.h"
#include "imgui.h"
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "config.h"
#include <chrono>
#include <random>

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
		mainWorld = createGameWorld();
		setupSystems();

        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

		auto setupCamera = [this](Saga::Entity player) {
			Saga::Entity camera = mainWorld->createEntity();
			std::shared_ptr<GraphicsEngine::Camera> defaultCamera 
				= std::make_shared<GraphicsEngine::Camera>();
			Saga::Camera& camComponent = *mainWorld->emplace<Saga::Camera>(camera, Saga::Camera{
				defaultCamera,  // camera
				true, // clearColorBufferOnDraw
				Saga::Theme_Nostalgic::colors[3], // clearColor
				glm::vec3(0.2, 1, 1), // coefficients for ambient, diffuse, and specular
				"phong", //shader
			});

			camComponent.camera->setPos(glm::vec3(0,8,0));

			mainWorld->emplace<Saga::Transform>(camera);
			Application::ThirdPersonCamera &tpcamera = *mainWorld->emplace<Application::ThirdPersonCamera>(camera,
				mainWorld->getComponent<Saga::Transform>(player));

			return camera;
		};

		auto setupPlane = [this]() {
			Saga::Entity plane = mainWorld->createEntity();
			Saga::Mesh& planeMesh = *mainWorld->emplace<Saga::Mesh>(plane, "Resources/Meshes/environment3.obj");
			Saga::Material& mat = *mainWorld->emplace<Saga::Material>(plane,
				Saga::Theme_Nostalgic::colors[1]);
            Saga::Transform* transform = mainWorld->emplace<Saga::Transform>(plane);

			mainWorld->emplace<Saga::Collider>(plane);
			mainWorld->emplace<Saga::MeshCollider>(plane);

			return plane;
		};

		auto setupLights = [this]() {
			Saga::Entity lightEnt = mainWorld->createEntity();
			Saga::Light& light = *mainWorld->emplace<Saga::Light>(lightEnt, GraphicsEngine::LightType::DIRECTIONAL, glm::normalize(glm::vec3(-1, -1, 0)));
			Saga::Transform& transform = *mainWorld->emplace<Saga::Transform>(lightEnt);
			return lightEnt;
		};

		auto setupPlayer = [this]() {
			Saga::Entity player = mainWorld->createEntity();
			mainWorld->emplace<Application::PlayerInput>(player);
			mainWorld->emplace<Platformer::PlayerController>(player, 5);
			mainWorld->emplace<Saga::Material>(player, Saga::Theme_Nostalgic::colors[0]);
			mainWorld->emplace<Saga::Mesh>(player, Saga::Mesh::StandardType::Sphere);
			mainWorld->emplace<Saga::Collider>(player);
			mainWorld->emplace<Saga::EllipsoidCollider>(player, glm::vec3(0.5f)); // sphere collider
			mainWorld->emplace<Saga::RigidBody>(player);
            mainWorld->emplace<Saga::CylinderCollider>(player, 1, 0.5);

			Saga::Transform* transform = mainWorld->emplace<Saga::Transform>(player);
			transform->transform->setPos(glm::vec3(0,10,0));
			return player;
		};

        auto setupFriend = [this, &rng]() {
            Saga::Entity fr = mainWorld->createEntity();
            float movespeed = std::uniform_real_distribution<float>(4.f, 9.f)(rng);
            float orbitDistance = std::uniform_real_distribution<float>(2,3)(rng);
            int colorIndex = std::uniform_int_distribution<int>(0,8)(rng);

            mainWorld->emplace<Platformer::FriendController>(fr, 
                movespeed, orbitDistance
            );
			mainWorld->emplace<Saga::Material>(fr, Saga::Theme_Nostalgic::colors[colorIndex]);
			mainWorld->emplace<Saga::Mesh>(fr, Saga::Mesh::StandardType::Sphere);
			mainWorld->emplace<Saga::Collider>(fr);
			mainWorld->emplace<Saga::EllipsoidCollider>(fr, glm::vec3(0.5f)); // sphere collider
			mainWorld->emplace<Saga::RigidBody>(fr);
            mainWorld->emplace<Saga::CylinderCollider>(fr, 1, 0.5);

            float randomRadiant = std::uniform_real_distribution<float>(0.0, 2*M_PI)(rng);

            glm::vec3 spawnPos = glm::vec3(sin(randomRadiant),0,cos(randomRadiant)) * orbitDistance;
            spawnPos.y = 5;

			Saga::Transform* transform = mainWorld->emplace<Saga::Transform>(fr);
			transform->transform->setPos(spawnPos);
			return fr;

        };

		auto setupBackingTrack = [this]() {
			Saga::Entity backingTrack = mainWorld->createEntity();
            mainWorld->emplace<Saga::AudioEmitter>(backingTrack, "event:/Loop", true);
			mainWorld->emplace<Saga::Transform>(backingTrack);
			return backingTrack;
		};

        auto setupNavMesh = [this]() {
            Saga::Entity navMeshContainer = mainWorld->createEntity();
            auto navmesh = mainWorld->emplace<Saga::NavMeshData>(navMeshContainer);
            navmesh->buildFromFile("Resources/Meshes/environment3nav.obj");
            mainWorld->emplace<Saga::Transform>(navMeshContainer)->transform->setPos(glm::vec3(0,0.1,0));
            mainWorld->emplace<Saga::Mesh>(navMeshContainer, "Resources/Meshes/environment3nav.obj");
            mainWorld->emplace<Saga::Material>(navMeshContainer, glm::vec3(0,0,0.5));
        };

		Saga::Entity plane = setupPlane();
		Saga::Entity light = setupLights();
		Saga::Entity player = setupPlayer();
		Saga::Entity camera = setupCamera(player);
		Saga::Entity backingTrack = setupBackingTrack();

        int friendCnt = 0;
        while (friendCnt --> 0) 
            setupFriend();

        setupNavMesh();
	}

	void App::setupSystems() {
		Saga::Systems::registerDrawSystem(mainWorld);
		Saga::Systems::registerCollisionSystem(mainWorld);
        Saga::Systems::registerAISystems(mainWorld);
		Platformer::Systems::registerPlayerControllerSystem(mainWorld);
        Platformer::Systems::registerSimpleTestAISystem(mainWorld);
		Platformer::Systems::registerFriendControllerSystem(mainWorld);
		Application::Systems::registerThirdPersonCameraSystem(mainWorld);

		Saga::Systems::setupAudioSystem(mainWorld);

		auto& systems = mainWorld->getSystems();

		// draw system
		systems.addStagedSystem(Saga::System<>(Saga::Systems::drawSystem), Saga::SystemManager::Stage::Draw);
		// draw on resize
		systems.addWindowResizeSystem(Saga::Systems::drawSystem_OnResize);

		// input reading
		systems.addKeyboardEventSystem(GLFW_KEY_W, Application::Systems::playerInputSystem_OnUpButton);
		systems.addKeyboardEventSystem(GLFW_KEY_A, Application::Systems::playerInputSystem_OnLeftButton);
		systems.addKeyboardEventSystem(GLFW_KEY_S, Application::Systems::playerInputSystem_OnDownButton);
		systems.addKeyboardEventSystem(GLFW_KEY_D, Application::Systems::playerInputSystem_OnRightButton);
		systems.addKeyboardEventSystem(GLFW_KEY_SPACE, Application::Systems::playerInputSystem_OnJumpButton);

		systems.addStagedSystem(Saga::System<float, float>(Application::Systems::thirdPersonCameraSystem), Saga::SystemManager::Stage::LateUpdate);
		systems.addMousePosSystem(Application::Systems::thirdPersonCameraSystem_OnMousePos);

		systems.addStagedSystem(Saga::System<float, float>(Platformer::Systems::playerControllerSystem));
		systems.addStagedSystem(Saga::System<float, float>(Platformer::Systems::friendControllerSystem));
	}

    void App::update(float deltaTime, float time) {
        Saga::App::update(deltaTime, time);

        auto setupFriend = [this](glm::vec3 pos) {
            Saga::Entity fr = mainWorld->createEntity();

			mainWorld->emplace<Saga::Material>(fr, Saga::Theme_Nostalgic::colors[1]);
			mainWorld->emplace<Saga::Mesh>(fr, Saga::Mesh::StandardType::Sphere);

			Saga::Transform* transform = mainWorld->emplace<Saga::Transform>(fr);
			transform->transform->setPos(pos + glm::vec3(0,0.5f,0));

            Saga::BehaviourTree* behaviourTree = mainWorld->emplace<Saga::BehaviourTree>(fr);
            behaviourTree
                ->makeRoot<Saga::BehaviourTreeNodes::SequenceNode>()
                ->makeChild<Platformer::LoadPlayerPosition>("player_position")
                ->makeChild<Platformer::ComputePathTowards>("player_position", "path")
                ->makeChild<Platformer::WalkPath>("path", "movement_speed")
                ->makeChild<Saga::BehaviourTreeNodes::Wait>(4);

            Saga::Blackboard* blackboard = mainWorld->emplace<Saga::Blackboard>(fr);

            blackboard->put("movement_speed", 2.0f);

			return fr;
        };

        ImGui::Begin("Friend Creation Tool", NULL, ImGuiWindowFlags_MenuBar);
        if (ImGui::Button("Spawn Random Friend")) {
            // chooses a random point on the navmesh
            for (auto navData : *mainWorld->viewAll<Saga::NavMeshData>()) {
                std::optional<glm::vec3> spawnPos = navData.getRandomPosition();
                if (spawnPos) {
                    setupFriend(spawnPos.value());
                    break; // we only spawn it on the first navmesh
                }
            }
        }
        ImGui::End();
    }
}
