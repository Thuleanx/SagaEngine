#pragma once

#include "behaviourtree.h"

namespace Saga {
    namespace BehaviourTreeNodes {
        class CompositeNode : public BehaviourTree::BehaviourTreeNode {
            protected:
                std::vector<std::shared_ptr<BehaviourTreeNode>> children;
                std::optional<int> lastRunningChild;
            public:
                CompositeNode& addChild(std::shared_ptr<BehaviourTreeNode> child);
                void reset();
        };

        class SelectorNode : public CompositeNode {
            public:
                virtual BehaviourTree::Status update(float seconds, BehaviourTree::Blackboard& blackboard, bool updatedLastFrame) override;
        };

        class SequenceNode : public CompositeNode {
            public:
                virtual BehaviourTree::Status update(float seconds, BehaviourTree::Blackboard& blackboard, bool updatedLastFrame) override;
        };

    }
}
