#pragma once

#include "Engine/Components/BehaviourTree/blackboard.h"
#include "behaviourtree.h"
#include <memory>

namespace Saga {
    namespace BehaviourTreeNodes {
        class CompositeNode : public BehaviourTree::Node, public std::enable_shared_from_this<CompositeNode> {
            protected:
                std::vector<std::shared_ptr<Node>> children;
                std::optional<int> lastRunningChild;
            public:
                std::shared_ptr<CompositeNode> addChild(std::shared_ptr<Node> child);

                template <typename T, typename ... Args>
                std::shared_ptr<CompositeNode> makeChild(Args &&... args) {
                    static_assert(std::is_base_of<Node, T>::value, "Type assigned to child must be a Behaviour tree node");
                    children.emplace_back(std::make_shared<T>(args...));
                    return shared_from_this();
                }

                void reset();
        };

        class SelectorNode : public CompositeNode {
            public:
                virtual BehaviourTree::Status update(float seconds, Blackboard& blackboard, bool updatedLastFrame) override;
        };

        class SequenceNode : public CompositeNode {
            public:
                virtual BehaviourTree::Status update(float seconds, Blackboard& blackboard, bool updatedLastFrame) override;
            private:
                bool ranChildLastFrame;
        };

    }
}
