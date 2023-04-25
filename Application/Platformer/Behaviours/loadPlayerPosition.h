#pragma once

#include "Engine/Components/BehaviourTree/behaviourtree.h"
#include "Engine/Components/BehaviourTree/tasknodes.h"

namespace Platformer {
    class LoadPlayerPosition : public Saga::BehaviourTreeNodes::Task {
    public:
        LoadPlayerPosition(const std::string &key);

        virtual Saga::BehaviourTree::Status update(Saga::Blackboard& blackboard, bool executedLastFrame) override;
    private:
        const std::string key;
    };
}
