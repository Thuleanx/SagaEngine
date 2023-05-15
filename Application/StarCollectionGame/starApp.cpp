#include "starApp.h"
#include "Application/StarCollectionGame/ECS/player/player.h"
#include "Application/StarCollectionGame/builders/lighting.h"
#include "Application/StarCollectionGame/builders/player.h"
#include "Application/StarCollectionGame/builders/star.h"
#include "Application/StarCollectionGame/builders/terrain.h"
#include "Application/StarCollectionGame/config.h"
#include "Engine/Components/drawSystemData.h"
#include "Engine/Entity/entity.h"
#include "Engine/Systems/audioSystem.h"
#include "Engine/Systems/drawSystem.h"
#include "Engine/Systems/particleSystem.h"
#include "Engine/Systems/systemManager.h"
#include "Engine/Utils/colors/colorPalette.h"
#include "Engine/Utils/random.h"

namespace Star {

Saga::Palette palette = Saga::Palette();

StarApp::StarApp() {
    palette = Saga::Palette(paletteFilename);
    worldSetup();
}

StarApp::~StarApp() {
}

void StarApp::worldSetup() {
    mainWorld = createGameWorld();
    mainWorld->emplace<Saga::DrawSystemData>(mainWorld->getMasterEntity())->postProcessingSettings.fogColor = 
        palette.getColor(fogColorIndex);

    // system setups
    Saga::Systems::registerDrawSystem(mainWorld);
    Saga::Systems::registerCollisionSystem(mainWorld);
    Saga::Systems::registerParticleSystem(mainWorld);
    Saga::Systems::setupAudioSystem(mainWorld);
    Star::Systems::registerGroupsAndSystems(mainWorld);

    glm::vec3 lightColor = palette.getColor(lightColorIndex);

    createPlayer(mainWorld, glm::vec3(0,10,10));
    createDirectionalLight(mainWorld, glm::vec3(-1,-1,0), lightColor);

    createMainStage(mainWorld, glm::vec3(0,0,0));

    std::vector<glm::vec3> islandPos = {
        glm::vec3(25, 10, 25),
        glm::vec3(20, 3, 15),
        glm::vec3(-25, 5, -35),
        glm::vec3(-12, -2, -15),
        glm::vec3(18, -4, -15),
        glm::vec3(28, 2, 2),
        glm::vec3(-25, 1, 2),
        glm::vec3(0, -40, 0),
        glm::vec3(0, 0.5, 20),
    };

    std::vector<glm::vec3> extraStars = {
        glm::vec3(3,5,10),
        glm::vec3(2,2,22),
        glm::vec3(-19,2,29)/2.0f,
        glm::vec3(20,10,-28)/2.0f,
        glm::vec3(0,-5,0),
    };

    glm::vec3 starDisplacementMin = glm::vec3(-5, 2, -5);
    glm::vec3 starDisplacementMax = glm::vec3(5, 5, 5);

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

    for (glm::vec3 extraStar : extraStars) {
        createStar(mainWorld, extraStar);
    }

    int maxStars = extraStars.size() + islandPos.size();

    mainWorld->getSystems().addStagedSystem(
    Saga::System<float,float>([this, maxStars](std::shared_ptr<Saga::GameWorld> world, float, float) -> void {
        std::optional<Player*> player;
        if ((player = world->viewAll<Player>()->any())) {
            if (player.value()->starsCollected.size() == maxStars) {
                removeGameWorld(mainWorld);
                worldSetup();
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
