#pragma once

#include "Engine/Components/BehaviourTree/behaviourtree.h"
#include "Engine/Components/BehaviourTree/tasknodes.h"

namespace Platformer {
    class ComputePathTowards : public Saga::BehaviourTreeNodes::Task {
    public:
        ComputePathTowards(const std::string &targetKey, const std::string &pathKey);

        virtual Saga::BehaviourTree::Status update(float seconds, Saga::Blackboard& blackboard, bool executedLastFrame) override;
    private:
        const std::string targetKey;
        const std::string pathKey;
    };
}
