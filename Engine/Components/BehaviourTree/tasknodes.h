#pragma once

#include "Engine/Components/BehaviourTree/behaviourtree.h"

namespace Saga::BehaviourTreeNodes {
    class Task : public BehaviourTree::BehaviourTreeNode {};

    template <typename T>
    class SetVariable : public Task {
    public:
        SetVariable(const std::string &parameterName, const T value) 
            : parameterName(parameterName), value(value) {}
        SetVariable(const SetVariable &other) 
            : parameterName(other.parameterName), value(other.value) {}

        virtual BehaviourTree::Status update(float seconds, BehaviourTree::Blackboard& blackboard, bool executedLastFrame) override {
            blackboard.put(parameterName, value);
            return BehaviourTree::SUCCESS;
        }
    private:
        const std::string parameterName;
        const T value;
    };
}
