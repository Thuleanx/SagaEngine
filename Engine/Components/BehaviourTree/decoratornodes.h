#pragma once

#include "Engine/Components/BehaviourTree/behaviourtree.h"
#include <string>

namespace Saga::BehaviourTreeNodes {

    class Decorator : public BehaviourTree::BehaviourTreeNode {
        protected:
            std::shared_ptr<BehaviourTreeNode> child;
        public:
            Decorator& setChild(std::shared_ptr<BehaviourTreeNode> child);
    };

    class Inverse : public Decorator {
    public:
        virtual BehaviourTree::Status update(float seconds, BehaviourTree::Blackboard& blackboard, bool executedLastFrame) override;
    };

    class RepeatMultipleTimes : public Decorator {
    public:
        RepeatMultipleTimes();
        RepeatMultipleTimes(int maxCount);
        RepeatMultipleTimes(const RepeatMultipleTimes& limiter);

        virtual BehaviourTree::Status update(float seconds, BehaviourTree::Blackboard& blackboard, bool executedLastFrame) override;
    private:
        const int maxCount;
        int repeatCount = 0;
    };

    class Succeeder : public Decorator {
    public:
        virtual BehaviourTree::Status update(float seconds, BehaviourTree::Blackboard& blackboard, bool executedLastFrame) override;
    };

    class Repeat : public Decorator {
    public:
        virtual BehaviourTree::Status update(float seconds, BehaviourTree::Blackboard& blackboard, bool executedLastFrame) override;
    };

    class RepeatUntilFail: public Decorator {
    public:
        virtual BehaviourTree::Status update(float seconds, BehaviourTree::Blackboard& blackboard, bool executedLastFrame) override;
    };
}
