#include "tasknodes.h"
#include "Engine/_Core/logger.h"

namespace Saga::BehaviourTreeNodes {

    // Wait id starts at 0
    int Wait::newUID = 0;

    Wait::Wait(const float durationSeconds) 
        : durationSeconds(durationSeconds), uid(newUID++), timeleftBlackboardKey("@wait::" + std::to_string(uid)) {}
    Wait::Wait(const Wait& other) : Wait(other.durationSeconds) {}

    BehaviourTree::Status
    Wait::update(Blackboard &blackboard, bool executedLastFrame) {
        if (!executedLastFrame) blackboard.put(timeleftBlackboardKey, durationSeconds);

        std::optional<float> result = blackboard.get<float>(timeleftBlackboardKey);
        if (!result || result.value() < 0)
            return BehaviourTree::SUCCESS;

        // decrement the time left
        blackboard.put(timeleftBlackboardKey, result.value() - blackboard.deltaTime);

        return BehaviourTree::RUNNING;
    }
}
