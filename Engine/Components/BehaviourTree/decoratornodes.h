#pragma once

#include "Engine/Components/BehaviourTree/behaviourtree.h"
#include <string>

namespace Saga::BehaviourTreeNodes {

    /**
     * @brief A node with at most one child and purely modifies the child's return status.
     */
    class Decorator : public BehaviourTree::Node {
        protected:
            std::shared_ptr<Node> child; //!< the child.
        public:
            /**
             * @brief Set the node's child.
             *
             * @param child the new child.
             *
             * @return a pointer to the child for operator chaining.
             */
            std::shared_ptr<Node> setChild(std::shared_ptr<Node> child);

            /**
             * @brief Create a new child node and override the previous.
             *
             * @tparam T the type of the child node.
             * @tparam Args the type of arguments to the child node's constructor.
             *
             * @param args the arguments to the child's constructor.
             *
             * @return std::shared_ptr<T> to the child node.
             */
            template <typename T, typename ... Args>
            std::shared_ptr<T> makeChild(Args &&... args) {
                static_assert(std::is_base_of<Node, T>::value, "Type assigned to child must be a Behaviour tree node");
                std::shared_ptr<T> child = std::make_shared<T>(args...);
                this->child = child;
                return child;
            }
    };

    /**
     * @brief Node that inverts the child's return status. Useful as a NOT operator for Conditionals.
     */
    class Inverse : public Decorator {
    public:
        /**
         * @brief Run the child if it exists, and invert the run status.
         *
         * @param blackboard the blackboard used to pass onto the child node.
         * @param updatedLastFrame used to determine if information on the node is stale.
         *  for this node, this isn't needed.
         *
         * @return FAIL if no child exist, or if the child's run status is SUCCESS.
         * @return SUCCESS if the child's run status is FAIL.
         * @return RUNNING If the child's run status is RUNNING, effectively not doing anything.
         */
        virtual BehaviourTree::Status update(Blackboard& blackboard, bool updatedLastFrame) override;
    };

    /**
     * @brief Node that runs the child a fixed amount of time before returning SUCCESS.
     */
    class RepeatMultipleTimes : public Decorator {
    public:
        /**
         * @brief Create a RepeatMultipleTimes node with the specified runtime count.
         *
         * @param runCount the number of times to run the child node.
         */
        RepeatMultipleTimes(int runCount);

        /**
         * @brief Copy constructor for RepeatMultipleTimes.
         *
         * @param other the other object to copy.
         */
        RepeatMultipleTimes(const RepeatMultipleTimes& other);


        /**
         * @brief Runs the child the specified amount of times.
         *
         * @param blackboard the blackboard used to pass onto the child node.
         * @param updatedLastFrame used to determine if information on the node is stale.
         *      For this node, this is needed to know when to reset the repeatCount to 0.
         *
         * @return FAIL if the child node either does not exist, or return FAIL upon updating.
         * @return SUCCESS if the child node has ran and returned SUCCESS for the specified amount of time.
         * @return RUNNING otherwise.
         */
        virtual BehaviourTree::Status update( Blackboard& blackboard, bool updatedLastFrame) override;
    private:
        const int runCount; //!< Number of times to repeat the child.

        //TODO: it's probably better to make this variable be on the blackboard.
        int repeatCount = 0; //!< Number of times the child node has ran. 
    };

    /**
     * @brief Node that always return SUCCESS. Useful for having multiple tasks run at once.
     */
    class Succeeder : public Decorator {
    public:
        /**
         * @brief Executes the child node if it exists.
         *
         * @param blackboard to pass onto its child.
         * @param updatedLastFrame to pass to its child.
         *
         * @return SUCCESS.
         */
        virtual BehaviourTree::Status update(Blackboard& blackboard, bool updatedLastFrame) override;
    };

    /**
     * @brief Node that keeps runnning the child. Useful for modelling states that should last until 
     * interrupted by another higher priority state.
     */
    class Repeat : public Decorator {
    public:
        /**
         * @brief Run the child node.
         *
         * @param blackboard to pass onto its child.
         * @param updatedLastFrame used to determine if information on the node is stale.
         *
         * @return RUNNING.
         */
        virtual BehaviourTree::Status update(Blackboard& blackboard, bool updatedLastFrame) override;
    };

    /**
     * @brief Node that keeps running the child until it fails.
     */
    class RepeatUntilFail: public Decorator {
    public:
        /**
         * @brief Run the child node.
         *
         * @param blackboard to pass onto its child.
         * @param updatedLastFrame used to determine if information on the node is stale.
         *
         * @return FAIL if the child fails.
         * @return RUNNING otherwise.
         */
        virtual BehaviourTree::Status update(Blackboard& blackboard, bool updatedLastFrame) override;
    };
}
