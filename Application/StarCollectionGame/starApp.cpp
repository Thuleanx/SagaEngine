#include "starApp.h"
#include "Application/StarCollectionGame/ECS/player/player.h"
#include "Application/StarCollectionGame/builders/lighting.h"
#include "Application/StarCollectionGame/builders/player.h"
#include "Application/StarCollectionGame/builders/star.h"
#include "Application/StarCollectionGame/builders/terrain.h"
#include "Engine/Entity/entity.h"
#include "Engine/Systems/audioSystem.h"
#include "Engine/Systems/drawSystem.h"
#include "Engine/Systems/particleSystem.h"
#include "Engine/Systems/systemManager.h"
#include "Engine/Utils/random.h"

namespace Star {

StarApp::StarApp() {
    worldSetup();
}

StarApp::~StarApp() {
}

void StarApp::worldSetup() {
    mainWorld = createGameWorld();

    // system setups
    Saga::Systems::registerDrawSystem(mainWorld);
    Saga::Systems::registerCollisionSystem(mainWorld);
    Saga::Systems::registerParticleSystem(mainWorld);
    Saga::Systems::setupAudioSystem(mainWorld);
    Star::Systems::registerGroupsAndSystems(mainWorld);

    createPlayer(mainWorld, glm::vec3(0,10,10));
    createDirectionalLight(mainWorld, glm::vec3(-1,-1,0), glm::vec3(0.5,0.5,1));

    createStar(mainWorld, glm::vec3(0,5,0));
    createMainStage(mainWorld, glm::vec3(0,0,0));

    std::vector<glm::vec3> islandPos = {
        glm::vec3(30, 10, 30),
        glm::vec3(-30, 5, -40),
        glm::vec3(12, 8, 20),
        glm::vec3(20, -4, -20),
    };

    glm::vec3 starDisplacementMin = glm::vec3(-5, 2, -5);
    glm::vec3 starDisplacementMax = glm::vec3(5, 10, 5);

    for (glm::vec3 pos : islandPos) {
        glm::vec3 displacement = glm::vec3(
            Saga::Random::getUniformRandom01(),
            Saga::Random::getUniformRandom01(),
            Saga::Random::getUniformRandom01()
        );
        displacement = displacement * (starDisplacementMax - starDisplacementMin) + starDisplacementMin;
        createStar(mainWorld, pos + displacement);
        createSubStage(mainWorld, pos);
    }

    mainWorld->getSystems().addStagedSystem(
    Saga::System<float,float>([this](std::shared_ptr<Saga::GameWorld> world, float, float) -> void {
        std::optional<Player*> player;
        if ((player = world->viewAll<Player>()->any())) {
            if (player.value()->numStarsCollected == 10) {
                removeGameWorld(mainWorld);
                endingWorldSetup();
            }
        }
    }),
    Saga::SystemManager::Stage::LateUpdate);
}

void StarApp::endingWorldSetup() {
    mainWorld = createGameWorld();

    Saga::Systems::registerDrawSystem(mainWorld);
    Saga::Systems::registerParticleSystem(mainWorld);
    Saga::Systems::setupAudioSystem(mainWorld);
}

}
