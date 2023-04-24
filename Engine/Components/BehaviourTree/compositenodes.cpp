#include "compositenodes.h"
#include "Engine/_Core/logger.h"

namespace Saga::BehaviourTreeNodes {

    std::shared_ptr<CompositeNode> CompositeNode::addChild(std::shared_ptr<Node> child) {
        children.push_back(child);
        return shared_from_this();
    }

    void CompositeNode::reset() { lastRunningChild = {}; }

    BehaviourTree::Status
    SelectorNode::update(Blackboard &blackboard,bool updatedLastFrame) {
        if (!updatedLastFrame) reset();

        // loop through children.
        for (int i = 0; i < children.size(); i++) {
            std::shared_ptr<Node> child = children[i];

            bool updatedChildLastFrame = lastRunningChild && lastRunningChild.value() == i;
            BehaviourTree::Status result = child->update(blackboard, updatedChildLastFrame);

            // continue updating if a child fail
            if (result == BehaviourTree::FAIL) continue;

            if (result == BehaviourTree::RUNNING)
                lastRunningChild = i;
            else reset();

            // stop update if there's a child that's running or success
            return result;
        }

        reset();
        // fail only if all child fails
        return BehaviourTree::FAIL;
    }

    BehaviourTree::Status
    SequenceNode::update(Blackboard &blackboard, bool updatedLastFrame) {
        if (!updatedLastFrame)
            reset();
        // reset if lastRunningChild is pointing to an invalid index
        if (lastRunningChild && (lastRunningChild.value() < 0 || lastRunningChild.value() >= children.size()))
            reset();

        // useful to determine if the child we run this frame has stale information
        bool ranChildLastFrame = lastRunningChild.has_value();
        if (!lastRunningChild) lastRunningChild = 0;

        BehaviourTree::Status result = BehaviourTree::SUCCESS;

        // we keep running if success. This means that it's good practice
        // to put success checks first thing in your task node, then the actual behaviour.
        while (result == BehaviourTree::SUCCESS) {
            result = children[lastRunningChild.value()]->update(blackboard, ranChildLastFrame);

            if (result == BehaviourTree::SUCCESS) {
                lastRunningChild.value()++;
                if (lastRunningChild == children.size())  {
                    reset();
                    return BehaviourTree::SUCCESS;
                }
            }
            ranChildLastFrame = false;
        }

        if (result == BehaviourTree::FAIL)
            reset();
        return result;
    }

} // namespace Saga
