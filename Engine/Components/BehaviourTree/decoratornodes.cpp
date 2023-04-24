#include "decoratornodes.h"
#include "Engine/Components/BehaviourTree/behaviourtree.h"

namespace Saga::BehaviourTreeNodes {

    Decorator& Decorator::setChild(std::shared_ptr<BehaviourTreeNode> child) {
        this->child = child;
    }

    // INVERSE
    BehaviourTree::Status Inverse::update(float seconds, BehaviourTree::Blackboard& blackboard, bool updatedLastFrame) {
        if (child) {
            BehaviourTree::Status childRunStatus = child->update(seconds, blackboard, updatedLastFrame);

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
    RepeatMultipleTimes::RepeatMultipleTimes() : maxCount(0), repeatCount(0) {}
    RepeatMultipleTimes::RepeatMultipleTimes(int maxCount) : maxCount(maxCount), repeatCount(0) {}
    RepeatMultipleTimes::RepeatMultipleTimes(const RepeatMultipleTimes &other) : maxCount(other.maxCount), repeatCount(other.repeatCount) {}

    BehaviourTree::Status RepeatMultipleTimes::update(float seconds, BehaviourTree::Blackboard &blackboard, bool updatedLastFrame) {
        if (!updatedLastFrame) repeatCount = 0;

        if (repeatCount < maxCount) {
            BehaviourTree::Status status = child->update(seconds, blackboard, updatedLastFrame);
            if (status == BehaviourTree::FAIL)
                return BehaviourTree::FAIL;
            repeatCount += status == BehaviourTree::SUCCESS;
        }

        if (repeatCount == maxCount) {
            repeatCount = 0;
            return BehaviourTree::SUCCESS;
        }
        return BehaviourTree::RUNNING;
    }

    // SUCCEEDER
    BehaviourTree::Status Succeeder::update(float seconds, BehaviourTree::Blackboard& blackboard, bool updatedLastFrame) {
        if (child) child->update(seconds, blackboard, updatedLastFrame);
        return BehaviourTree::SUCCESS;
    }

    // REPEATER 
    BehaviourTree::Status Repeat::update(float seconds, BehaviourTree::Blackboard& blackboard, bool updatedLastFrame) {
        if (child)
            child->update(seconds, blackboard, updatedLastFrame);
        return BehaviourTree::RUNNING;
    }

    // REPEAT UNTIL FAIL
    BehaviourTree::Status RepeatUntilFail::update(float seconds, BehaviourTree::Blackboard& blackboard, bool updatedLastFrame) {
        if (child && child->update(seconds, blackboard, updatedLastFrame) == BehaviourTree::FAIL)
            return BehaviourTree::FAIL;
        return BehaviourTree::RUNNING;
    }
}
