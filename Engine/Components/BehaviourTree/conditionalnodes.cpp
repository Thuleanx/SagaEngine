#include "conditionalNodes.h"
#include "Engine/Components/BehaviourTree/behaviourtree.h"

namespace Saga::BehaviourTreeNodes {

    BehaviourTree::Status Conditional::update(float seconds, BehaviourTree::Blackboard &blackboard, bool updatedLastFrame) {
        if (!evaluate(blackboard)) {
            skipChildLastFrame = true;
            return BehaviourTree::FAIL;
        }
        if (child) {
            BehaviourTree::Status status = child->update(seconds, blackboard, updatedLastFrame && !skipChildLastFrame);
            skipChildLastFrame = false;
            return status;
        }
        return BehaviourTree::SUCCESS;
    }
}
