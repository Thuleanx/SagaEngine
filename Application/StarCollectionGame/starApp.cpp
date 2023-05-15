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

    createPlayer(mainWorld, glm::vec3(0,10,10));
    createMainStage(mainWorld, glm::vec3(0,0,0));
    createDirectionalLight(mainWorld, glm::vec3(-1,-1,0), glm::vec3(0.5,0.5,1));
    createStar(mainWorld, glm::vec3(0,5,0));
    createSubStage(mainWorld, glm::vec3(30, 10, 30));
}

}
