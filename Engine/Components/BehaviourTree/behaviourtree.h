#pragma once

#include <vector>
#include <memory>
#include <map>
#include <any>
#include <optional>
#include "Engine/Components/BehaviourTree/blackboard.h"

namespace Saga {
    class GameWorld;

    /**
     * @brief Represents a behaviour tree.
     *
     * @ingroup component
     */
    class BehaviourTree {
    public:
        /**
         * @brief Status of behaviour tree nodes, which is returned to the parent nodes
         * and determines whether or not a node gets executed. 
         * 
         * It can have many meanings. For instance, a conditional node can have SUCCESS
         * be a standin for true, and FAIL false.
         */
        enum Status {
            SUCCESS,
            FAIL, 
            RUNNING //!< Signals that a node is still processing.
        };

        /**
         * @brief A generic behaviour tree node, with a tick method.
         */
        class Node {
            public:
                /**
                 * @brief Update the tree node.
                 *
                 * @param blackboard a map of string to data that can be used for the node to perform computations
                 * and store persistent values.
                 * @param updatedLastFrame tells the node whether it was on the path of execution last frame.
                 *  If not, it knows its local information is stale and needs to be reset.
                 */
                virtual Status update(Blackboard &blackboard, bool updatedLastFrame) = 0;
        };

        /**
         * @brief Update the behaviour tree. This involves running the root node's update.
         *
         * @param blackboard the blackboard used for data communication and persistent data storage.
         */
        void update(Blackboard& blackboard);

        /**
         * @brief Make a root node to the tree. This creates / emplaces it, and return it back to the user.
         * 
         * @tparam T the type of the root node. This can be anything that inherits from BehaviourTree::Node.
         * @tparam Args the type of any arguments that goes into construction said root node.
         *
         * @param args the arguments used to construct the root node.
         * @return std::shared_ptr<T> to the root node, so that one can chain create more nodes from the root.
         */
        template <typename T, typename ...Args>
        std::shared_ptr<T> makeRoot(Args &&... args) {
            static_assert(std::is_base_of<Node, T>::value, "Type assigned to root must be a Behaviour tree node");
            firstFrame = true;
            std::shared_ptr<T> newRootPtr = std::make_shared<T>(args...);
            root = newRootPtr;
            return newRootPtr;
        }

        /**
         * @brief Set the root node.
         *
         * @param rootNode a pointer to a new root node to replace the current one.
         */
        void setRoot(std::shared_ptr<Node> rootNode);
    private:
        bool firstFrame = true; //!< True if the first update on the current root has not ran yet. 
                                //This is helpful for the root to determine if its information is stale.
        std::shared_ptr<Node> root; //!< The tree's root node.
    };
}
