#pragma once

#include "Engine/Components/BehaviourTree/behaviourtree.h"
#include <string>

namespace Saga::BehaviourTreeNodes {

    class Decorator : public BehaviourTree::Node {
        protected:
            std::shared_ptr<Node> child;
        public:
            std::shared_ptr<Node> setChild(std::shared_ptr<Node> child);

            template <typename T, typename ... Args>
            std::shared_ptr<T> makeChild(Args &&... args) {
                static_assert(std::is_base_of<Node, T>::value, "Type assigned to child must be a Behaviour tree node");
                return child = std::make_shared<T>(args...);
            }
    };

    class Inverse : public Decorator {
    public:
        virtual BehaviourTree::Status update(float seconds, Blackboard& blackboard, bool executedLastFrame) override;
    };

    class RepeatMultipleTimes : public Decorator {
    public:
        RepeatMultipleTimes();
        RepeatMultipleTimes(int maxCount);
        RepeatMultipleTimes(const RepeatMultipleTimes& limiter);

        virtual BehaviourTree::Status update(float seconds, Blackboard& blackboard, bool executedLastFrame) override;
    private:
        const int maxCount;
        int repeatCount = 0;
    };

    class Succeeder : public Decorator {
    public:
        virtual BehaviourTree::Status update(float seconds, Blackboard& blackboard, bool executedLastFrame) override;
    };

    class Repeat : public Decorator {
    public:
        virtual BehaviourTree::Status update(float seconds, Blackboard& blackboard, bool executedLastFrame) override;
    };

    class RepeatUntilFail: public Decorator {
    public:
        virtual BehaviourTree::Status update(float seconds, Blackboard& blackboard, bool executedLastFrame) override;
    };
}
