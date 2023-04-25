#include "computePathTowards.h"
#include "Application/Platformer/Components/simpleTestAI.h"
#include "Engine/Components/Navigation/navMeshData.h"
#include "Engine/Components/transform.h"
#include "Engine/Gameworld/gameworld.h"
#include "Engine/_Core/logger.h"
#include "glm/gtx/string_cast.hpp"

namespace Platformer {
    ComputePathTowards::ComputePathTowards(const std::string &targetKey, const std::string &pathKey) : targetKey(targetKey), pathKey(pathKey) { }
    
    Saga::BehaviourTree::Status ComputePathTowards::update(Saga::Blackboard& blackboard, bool executedLastFrame) {
        auto world = blackboard.world;

        auto toPos = blackboard.get<glm::vec3>(targetKey);
        if (!toPos) return Saga::BehaviourTree::FAIL;

        auto navMeshData = world->viewAll<Saga::NavMeshData>()->any();
        if (!navMeshData) return Saga::BehaviourTree::FAIL;

        Saga::Transform* transform = world->getComponent<Saga::Transform>(blackboard.entity);
        if (!transform) return Saga::BehaviourTree::FAIL;

        std::optional<Saga::NavMesh::Path> path = 
            navMeshData.value()->findPath(transform->getPos(), toPos.value(), 0);

        if (!path) return Saga::BehaviourTree::FAIL;

        Saga::NavMesh::WalkablePath walkablePath = navMeshData.value()->tracePath(path.value());

        blackboard.put(pathKey, walkablePath);
        /* STRACE("computed path to player of length %f ending at %s", path->length, glm::to_string(path->to).c_str()); */

        return Saga::BehaviourTree::SUCCESS;
    }
}

