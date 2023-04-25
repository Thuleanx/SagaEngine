#include "loadPlayerPosition.h"
#include "Application/Platformer/Components/playerController.h"
#include "Engine/Components/BehaviourTree/behaviourtree.h"
#include "Engine/Components/transform.h"
#include "Engine/Gameworld/gameworld.h"
#include "Engine/_Core/logger.h"
#include "glm/gtx/string_cast.hpp"

namespace Platformer {
    LoadPlayerPosition::LoadPlayerPosition(const std::string& key) : key(key) { }

    Saga::BehaviourTree::Status LoadPlayerPosition::update(Saga::Blackboard& blackboard, bool executedLastFrame) {
        if (!blackboard.world) return Saga::BehaviourTree::FAIL;

        auto playerController = blackboard.world->viewAll<PlayerController>()->any();
        if (!playerController) return Saga::BehaviourTree::FAIL;

        Saga::Entity playerEntity = blackboard.world->getEntity(playerController.value());
        Saga::Transform* transform = blackboard.world->getComponent<Saga::Transform>(playerEntity);

        if (!transform) return Saga::BehaviourTree::FAIL;

        blackboard.put<glm::vec3>(key, transform->getPos());
        /* STRACE("loaded player position at %s", glm::to_string(transform->getPos()).c_str()); */

        return Saga::BehaviourTree::SUCCESS;
    }
}
