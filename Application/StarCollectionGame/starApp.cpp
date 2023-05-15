#include "starApp.h"
#include "Application/StarCollectionGame/ECS/player/player.h"
#include "Application/StarCollectionGame/builders/player.h"
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
    createPlayer(mainWorld, player);
}

}
