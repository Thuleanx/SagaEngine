#pragma once

#include "Engine/Components/BehaviourTree/behaviourtree.h"

namespace Saga::BehaviourTreeNodes {
    /**
     * @brief Leaf nodes of the behaviour tree.
     * Typically model actual actions that affect the game world.
     */
    class Task : public BehaviourTree::Node {};

    /**
     * @brief Node for setting a blackboard parameter to a specified value.
     *
     * @tparam T the type of the value stored in the blackboard parameter.
     */
    template <typename T>
    class SetVariable : public Task {
    public:
        /**
         * @brief Create a new SetVariable node.
         *
         * @param parameterName the name of the blackboard parameter.
         * @param value the value to set the blackboard parameter to.
         */
        SetVariable(const std::string &parameterName, const T value) 
            : parameterName(parameterName), value(value) {}
        /**
         * @brief Copy constructor for a SetVariable node.
         *
         * @param other the other SetVariable node.
         */
        SetVariable(const SetVariable &other) 
            : parameterName(other.parameterName), value(other.value) {}

        /**
         * @brief Sets the blackboard parameter to the specified value.
         *
         * @return SUCCESS
         */
        virtual BehaviourTree::Status update(Blackboard& blackboard, bool executedLastFrame) override {
            blackboard.put(parameterName, value);
            return BehaviourTree::SUCCESS;
        }
    private:
        const std::string parameterName; //!< the parameterName.
        const T value; //!< value to set the parameter to when this node gets updated.
    };

    /**
     * @brief Node for waiting for some specified realtime seconds.
     */
    class Wait : public Task {
    public:
        /**
         * @brief Create a Wait node with a specified duration.
         *
         * @param durationSeconds
         */
        Wait(const float durationSeconds);

        /**
         * @brief Copy constructor for a Wait node.
         *
         * @param other the Wait node to copy data from.
         *  this copies the duration but does not copy the uid of the other node.
         */
        Wait(const Wait& other);

        /**
         * @brief Waits for specified seconds.
         *
         * @param blackboard where this object stores the time left to wait.
         *
         * @return SUCCESS if the wait is completed.
         * @return RUNNING otherwise.
         */
        virtual BehaviourTree::Status update(Blackboard& blackboard, bool executedLastFrame) override;
    private:
        const int uid; //!< the uid of the node, used for constructing a unique timeleftBlackboardKey
        const float durationSeconds; //!< duration of the wait.
        const std::string timeleftBlackboardKey; //!< unique key used to store time remaining 
                                                 //of the wait to the blackboard.

        static int newUID; //!< Keeps track of the highest UID.
    };
}
