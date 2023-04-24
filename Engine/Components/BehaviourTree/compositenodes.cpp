#include "compositenodes.h"

namespace Saga::BehaviourTreeNodes {

    CompositeNode& CompositeNode::addChild(
            std::shared_ptr<BehaviourTreeNode> child) {
        children.push_back(child);
        return *this;
    }

    void CompositeNode::reset() { lastRunningChild = {}; }

    BehaviourTree::Status
    SelectorNode::update(float seconds,
            BehaviourTree::Blackboard &blackboard,
            bool updatedLastFrame) {
        if (!updatedLastFrame) reset();

        for (int i = 0; i < children.size(); i++) {
            std::shared_ptr<BehaviourTreeNode> child = children[i];

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
            BehaviourTree::Blackboard &blackboard,
            bool updatedLastFrame) {
        if (!updatedLastFrame) reset();
        if (lastRunningChild && (lastRunningChild.value() < 0 || lastRunningChild.value() >= children.size()))
            reset();

        if (!lastRunningChild) lastRunningChild = 0;

        BehaviourTree::Status result = children[lastRunningChild.value()]->update(seconds, blackboard, updatedLastFrame);

        if (result == BehaviourTree::SUCCESS && ++lastRunningChild.value() < children.size())
            return BehaviourTree::RUNNING;

        reset();
        return result;
    }

} // namespace Saga
