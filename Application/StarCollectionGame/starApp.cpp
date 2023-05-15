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

    Saga::Entity player = mainWorld->createEntity();
    createPlayer(mainWorld, player, glm::vec3(0,10,10));

    Saga::Entity mainStage = mainWorld->createEntity();
    createMainStage(mainWorld, mainStage, glm::vec3(0,0,0));

    Saga::Entity mainLight = mainWorld->createEntity();
    createDirectionalLight(mainWorld, mainLight, glm::vec3(-1,-1,0), glm::vec3(0.5,0.5,1));

    Saga::Entity star = mainWorld->createEntity();
    createStar(mainWorld, star, glm::vec3(0,5,0));
}

}
