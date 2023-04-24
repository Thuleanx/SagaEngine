#include "compositenodes.h"
#include "Engine/_Core/logger.h"

namespace Saga::BehaviourTreeNodes {

    std::shared_ptr<CompositeNode> CompositeNode::addChild(std::shared_ptr<Node> child) {
        children.push_back(child);
        return shared_from_this();
    }

    void CompositeNode::reset() { lastRunningChild = {}; }

    BehaviourTree::Status
    SelectorNode::update(float seconds,
            Blackboard &blackboard,
            bool updatedLastFrame) {
        if (!updatedLastFrame) reset();

        for (int i = 0; i < children.size(); i++) {
            std::shared_ptr<Node> child = children[i];

            bool updatedChildLastFrame = lastRunningChild && lastRunningChild.value() == i;
            BehaviourTree::Status result = child->update(
                    seconds, blackboard, updatedChildLastFrame);

            if (result == BehaviourTree::RUNNING)
                lastRunningChild = i;
            if (result == BehaviourTree::SUCCESS)
                reset();
            if (result != BehaviourTree::FAIL)
                return result;
        }

        reset();
        return BehaviourTree::FAIL;
    }

    BehaviourTree::Status
    SequenceNode::update(float seconds,
            Blackboard &blackboard,
            bool updatedLastFrame) {

        if (!updatedLastFrame) {
            reset();
            ranChildLastFrame = false;
        }

        if (lastRunningChild && (lastRunningChild.value() < 0 || lastRunningChild.value() >= children.size())) {
            reset();
            ranChildLastFrame = false;
        }

        if (!lastRunningChild) lastRunningChild = 0;

        BehaviourTree::Status result = children[lastRunningChild.value()]->update(seconds, blackboard, ranChildLastFrame);
        STRACE("Running: %d", lastRunningChild);
        ranChildLastFrame = true;

        if (result == BehaviourTree::SUCCESS && ++lastRunningChild.value() < children.size()) {
            ranChildLastFrame = false;
            return BehaviourTree::RUNNING;
        }
        else if (result == BehaviourTree::RUNNING)
            return BehaviourTree::RUNNING;

        reset();
        ranChildLastFrame = false;
        return result;
    }

} // namespace Saga
