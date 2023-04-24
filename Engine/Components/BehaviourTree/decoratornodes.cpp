#include "decoratornodes.h"
#include "Engine/Components/BehaviourTree/behaviourtree.h"

namespace Saga::BehaviourTreeNodes {

    std::shared_ptr<BehaviourTree::Node> Decorator::setChild(std::shared_ptr<Node> child) {
        this->child = child;
        return child;
    }

    // INVERSE
    BehaviourTree::Status Inverse::update(Blackboard& blackboard, bool updatedLastFrame) {
        if (child) {
            BehaviourTree::Status childRunStatus = child->update(blackboard, updatedLastFrame);

            switch (childRunStatus) {
                case BehaviourTree::RUNNING:
                    return BehaviourTree::RUNNING;
                case BehaviourTree::SUCCESS:
                    return BehaviourTree::FAIL;
                case BehaviourTree::FAIL:
                    return BehaviourTree::SUCCESS;
            };
        }

        return BehaviourTree::FAIL;
    }

    // LIMITER
    RepeatMultipleTimes::RepeatMultipleTimes(int maxCount) : runCount(maxCount), repeatCount(0) {}
    RepeatMultipleTimes::RepeatMultipleTimes(const RepeatMultipleTimes &other) : runCount(other.runCount), repeatCount(other.repeatCount) {}

    BehaviourTree::Status RepeatMultipleTimes::update(Blackboard &blackboard, bool updatedLastFrame) {
        if (!updatedLastFrame) repeatCount = 0;

        if (!child) return BehaviourTree::FAIL;

        if (repeatCount < runCount) {
            BehaviourTree::Status status = child->update(blackboard, updatedLastFrame);
            if (status == BehaviourTree::FAIL)
                return BehaviourTree::FAIL;
            repeatCount += status == BehaviourTree::SUCCESS;
        }

        if (repeatCount == runCount) {
            repeatCount = 0;
            return BehaviourTree::SUCCESS;
        }
        return BehaviourTree::RUNNING;
    }

    // SUCCEEDER
    BehaviourTree::Status Succeeder::update(Blackboard& blackboard, bool updatedLastFrame) {
        if (child) child->update(blackboard, updatedLastFrame);
        return BehaviourTree::SUCCESS;
    }

    // REPEATER 
    BehaviourTree::Status Repeat::update(Blackboard& blackboard, bool updatedLastFrame) {
        if (child)
            child->update(blackboard, updatedLastFrame);
        return BehaviourTree::RUNNING;
    }

    // REPEAT UNTIL FAIL
    BehaviourTree::Status RepeatUntilFail::update(Blackboard& blackboard, bool updatedLastFrame) {
        if (child && child->update(blackboard, updatedLastFrame) == BehaviourTree::FAIL)
            return BehaviourTree::FAIL;
        return BehaviourTree::RUNNING;
    }
}
