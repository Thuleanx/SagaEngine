#pragma once

#include "Engine/Components/BehaviourTree/behaviourtree.h"
#include "Engine/Components/BehaviourTree/decoratornodes.h"

namespace Saga::BehaviourTreeNodes {

    class Conditional : public Decorator {
        public:
            virtual BehaviourTree::Status update(float seconds, BehaviourTree::Blackboard& blackboard, bool executedLastFrame) override;
        protected:
            virtual bool evaluate(BehaviourTree::Blackboard& blackboard) = 0;
        private:
            bool skipChildLastFrame;
    };

    template <typename T>
    class IsNull : public Conditional {
        public:
            IsNull(const std::string &parameterName, const T value) 
                : parameterName(parameterName) {}
            IsNull(const IsNull &other) 
                : parameterName(other.parameterName) {}

            virtual bool evaluate(BehaviourTree::Blackboard& blackboard) override {
                std::optional<T> valueToTest = blackboard.get<T>(parameterName);
                return valueToTest && valueToTest.value() != nullptr;
            }
        private:
            std::string parameterName;
    };
}
