#include "simpleTestAISystem.h"
#include "Application/Platformer/Components/simpleTestAI.h"
#include "Engine/Components/navigation/navMeshData.h"
#include "Engine/Components/transform.h"
#include "Engine/Gameworld/gameworld.h"
#include "Engine/Systems/system.h"
#include "Engine/Systems/systemManager.h"

namespace Platformer::Systems {

void simpleTestAISystem(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time) {
    for (auto& [entity, testAI, transform] : *world->viewGroup<SimpleTestAI, Saga::Transform>()) {
        if (!testAI->path) {
            auto navMeshes = world->viewAll<Saga::NavMeshData>();
            if (navMeshes->size()) {
                Saga::NavMeshData& navMesh = *navMeshes->begin();
                std::optional<Saga::NavMesh::Path> optionalPath = navMesh.findPath(transform->getPos(), glm::vec3(0,0,0), 1.f);
                if (optionalPath) testAI->path = navMesh.tracePath(optionalPath.value());
            }
        }
        glm::vec3 curPos = transform->getPos();
        while (testAI->path->positions.size() && glm::distance(testAI->path->positions.back(), curPos) < 0.001f) testAI->path->positions.pop_back();
        // walk towards next waypoint
        glm::vec3 dir = testAI->path->positions.back() - curPos;
        float len = dir.length();
        if (len) dir /= len;

        float walkAmt = std::clamp(deltaTime * testAI->movementSpeed, 0.0f, len);

        glm::vec3 nxtPos = curPos + dir * walkAmt;
        transform->transform->setPos(nxtPos);
    }
}

void registerSimpleTestAISystem(std::shared_ptr<Saga::GameWorld> world) {
    world->registerGroup<SimpleTestAI, Saga::Transform>();
    world->getSystems().addStagedSystem(Saga::System<float,float>(Platformer::Systems::simpleTestAISystem), Saga::SystemManager::Stage::Update);
}

}
