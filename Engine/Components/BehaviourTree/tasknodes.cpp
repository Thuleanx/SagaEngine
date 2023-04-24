#include "tasknodes.h"
#include "Engine/_Core/logger.h"

namespace Saga::BehaviourTreeNodes {

    int Wait::newUID = 0;

    Wait::Wait(const float durationSeconds) 
        : durationSeconds(durationSeconds), uid(newUID++), timeleftBlackboardKey("@wait::" + std::to_string(newUID)) {}
    Wait::Wait() : Wait(0) {}
    Wait::Wait(const Wait& other) : Wait(other.durationSeconds) {}

    BehaviourTree::Status
    Wait::update(float seconds, Blackboard &blackboard, bool executedLastFrame) {
        STRACE("%f, %d, %s", durationSeconds, uid, timeleftBlackboardKey.c_str());
        if (!executedLastFrame) blackboard.put(timeleftBlackboardKey, durationSeconds);

        std::optional<float> result = blackboard.get<float>(timeleftBlackboardKey);
        STRACE("%f", result ? result.value() : 0);
        if (!result || result.value() < 0)
            return BehaviourTree::SUCCESS;

        if (result)
            blackboard.put(timeleftBlackboardKey, result.value() - seconds);

        return BehaviourTree::RUNNING;
    }
}
