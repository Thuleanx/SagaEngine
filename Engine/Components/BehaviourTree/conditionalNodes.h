#pragma once

#include "Engine/Components/BehaviourTree/behaviourtree.h"
#include "Engine/Components/BehaviourTree/blackboard.h"
#include "Engine/Components/BehaviourTree/decoratornodes.h"

namespace Saga::BehaviourTreeNodes {

    /**
     * @brief Represents a conditional node, that returns FAIL when the conditional is false.
     * If the conditional is true, then we will either return true, or if the Conditional has a child,
     * we execute that child and return its status.
     */
    class Conditional : public Decorator {
        public:
            /**
             * @brief Evaluate the condition. If the condition is true, additionally update the child if it 
             * exists.
             *
             * @param blackboard a map of string to data that can be used for the node to perform computations
             * and store persistent values.
             * @param updatedLastFrame used to determine if information on the node is stale.
             *
             * @return FAIL if the conditional evaluates to false.
             * @return SUCCESS if there's no child and the conditional evaluates to true.
             * @return the child's status otherwise.
             */
            virtual BehaviourTree::Status update(Blackboard& blackboard, bool executedLastFrame) override;
        protected:
            /**
             * @brief Generic condition meant to represent what this conditional's condition
             * is.
             *
             * @param blackboard a map of string to data that can be used for the node to perform computations
             * and store persistent values.
             */
            virtual bool evaluate(Blackboard& blackboard) = 0;
        private:
            bool skipChildLastFrame; //!< whether or not we evaluated the child in the last frame. The child is only
                                     //evaluated if the conditional evaluates to true.
    };

    /**
     * @brief Conditional node to test if a particular variable stored on the Blackboard is null
     * or not specified.
     */
    template <typename T>
    class IsNull : public Conditional {
        public:
            /**
             * @brief Create a new IsNull conditional node.
             *
             * @param parameterName the key to the parameter we want to test for null.
             */
            IsNull(const std::string &parameterName) 
                : parameterName(parameterName) {}

            /**
             * @brief Create a new IsNull conditional node from another copy.
             *
             * @param other the other IsNull conditional node.
             */
            IsNull(const IsNull &other) 
                : parameterName(other.parameterName) {}

            /**
             * @brief Function to evaluate the IsNull Conditional.
             * This simply loads a variable from the blackboard and test if its null.
             *
             * @blackboard the blackboard where the parameter is on.
             */
            virtual bool evaluate(Blackboard& blackboard) override {
                std::optional<T> valueToTest = blackboard.get<T>(parameterName);
                return !valueToTest || valueToTest.value() != nullptr;
            }
        private:
            std::string parameterName; //!< The name of the parameter we're testing for.
    };
}
