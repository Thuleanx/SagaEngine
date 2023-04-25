#include "walkPath.h"
#include "Engine/Components/transform.h"
#include "Engine/Datastructures/Navigation/navMesh.h"
#include "Engine/GameWorld/gameWorld.h"
#include "Engine/_Core/logger.h"

namespace Platformer {
    WalkPath::WalkPath(const std::string& pathKey,
            const std::string& speedKey) : pathKey(pathKey), speedKey(speedKey) {}

    Saga::BehaviourTree::Status WalkPath::update(Saga::Blackboard& blackboard, bool executedLastFrame) {
        if (!blackboard.world) return Saga::BehaviourTree::FAIL;

        std::optional<Saga::NavMesh::WalkablePath> path = blackboard.get<Saga::NavMesh::WalkablePath>(pathKey);
        std::optional<float> movementSpeed = blackboard.get<float>(speedKey);
        Saga::Transform* transform = blackboard.world->getComponent<Saga::Transform>(blackboard.entity);

        if (!path) return Saga::BehaviourTree::FAIL;
        if (!movementSpeed) return Saga::BehaviourTree::FAIL;
        if (!transform) return Saga::BehaviourTree::FAIL;

        glm::vec3 curPos = transform->getPos();

        while (path->positions.size() && glm::distance(path->positions.back(), curPos) < 0.001f) 
            path->positions.pop_back();

        blackboard.put(pathKey, path.value());

        if (!path->positions.size()) {
            return Saga::BehaviourTree::SUCCESS;
        }

        // walk towards next waypoint
        glm::vec3 dir = path->positions.back() - curPos;
        float len = glm::length(dir);
        if (len) dir /= len;

        float walkAmt = std::clamp(blackboard.deltaTime * movementSpeed.value(), 0.0f, len);

        glm::vec3 nxtPos = curPos + dir * walkAmt;
        transform->transform->setPos(nxtPos);


        return Saga::BehaviourTree::RUNNING;
    }
}
