#include "aiSystem.h"
#include "../GameWorld/gameworld.h"
#include "Engine/Components/BehaviourTree/behaviourtree.h"
#include "Engine/Components/BehaviourTree/blackboard.h"
#include "Engine/_Core/logger.h"

namespace Saga::Systems {

void registerAISystems(std::shared_ptr<GameWorld> world) {
    world->registerGroup<Saga::BehaviourTree, Saga::Blackboard>();
    world->getSystems().addStagedSystem(System<float,float>(AIupdateSystem), SystemManager::Stage::Update);
}

void AIupdateSystem(std::shared_ptr<GameWorld> world, float deltaTime, float time) {
    for (auto [entity, behaviourTree, blackboard] : *world->viewGroup<Saga::BehaviourTree, Saga::Blackboard>()) {
        blackboard->put("time", time);
        blackboard->put("deltaTime", deltaTime);
        behaviourTree->update(deltaTime, *blackboard);
    }
}

}
