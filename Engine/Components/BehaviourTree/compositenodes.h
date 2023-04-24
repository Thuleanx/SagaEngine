#pragma once

#include "Engine/Components/BehaviourTree/blackboard.h"
#include "behaviourtree.h"
#include <memory>

namespace Saga {
    namespace BehaviourTreeNodes {
        /**
         * @brief Nodes in the behaviour tree with multiple children.
         */
        class CompositeNode : public BehaviourTree::Node, public std::enable_shared_from_this<CompositeNode> {
            protected:
                std::vector<std::shared_ptr<Node>> children; //!< List of children.
                std::optional<int> lastRunningChild; //!< Last child that's running. 
                                                     //Useful for determining which child subtree has ran in the last frame.
            public:
                /**
                 * @brief Add a child to the Composite Node.
                 * This returns the composite node.
                 *
                 * @param child the child.
                 * @return a smart pointer to the composite node itself, for chaining multiple adds.
                 */
                std::shared_ptr<CompositeNode> addChild(std::shared_ptr<Node> child);

                /**
                 * @brief Insert / emplace a child into the composite node.
                 * This creates a child to the composite node, and add it as the last child.
                 *
                 * @tparam T the type of the child node.
                 * @tparam Args the type of arguments for the child's constructor.
                 *
                 * @param args the arguments used to create the child.
                 *
                 * @return a smart pointer to the composite node itself, for operator chaining.
                 */
                template <typename T, typename ... Args>
                std::shared_ptr<CompositeNode> makeChild(Args &&... args) {
                    static_assert(std::is_base_of<Node, T>::value, "Type assigned to child must be a Behaviour tree node");
                    children.emplace_back(std::make_shared<T>(args...));
                    return shared_from_this();
                }

                /**
                 * @brief Reset the current composite node. This resets the lastRunningChild to {}.
                 */
                void reset();
        };

        /**
         * @brief Node that updates all child nodes in order until one does not fail.
         */
        class SelectorNode : public CompositeNode {
            public:
                /**
                 * @brief Iterate through all children of the node and update them until one does not fail, effectively
                 * "selecting" one of them.
                 *
                 * If its children nodes are Conditionals, then this is effectively an OR gate.
                 *
                 * @param blackboard a map of string to data that can be used for the node to perform computations
                 * and store persistent values.
                 * @param updatedLastFrame used to determine if information on the node is stale.
                 *  In this case, the node resets if updatedLastFrame is false.
                 *
                 * @return SUCCESS if the first non failing child return SUCCESS.
                 * @return FAIL if all children return FAIL
                 * @return RUNNING if the first non failing child return RUNNING.
                 */
                virtual BehaviourTree::Status update(Blackboard& blackboard, bool updatedLastFrame) override;
        };

        /**
         * @brief Node that updates all its child nodes sequentially. If one fails, we return fail.
         * This effectively makes it an AND gate if all children are Conditionals.
         */
        class SequenceNode : public CompositeNode {
            public:
                /**
                 * @brief Iterate through all children sequentially and update them until one does fail, or the entire sequence
                 * successfully concluded. If a child returns success, we immediately go and execute the next one in the same
                 * frame.
                 *
                 * @param blackboard a map of string to data that can be used for the node to perform computations
                 * and store persistent values.
                 * @param updatedLastFrame used to determine if information on the node is stale.
                 *  In this case, the node resets if updatedLastFrame is false.
                 *
                 * @return SUCCESS if the sequence completes running.
                 * @return FAIL if a child fails to complete.
                 * @return RUNNING otherwise.
                 */
                virtual BehaviourTree::Status update(Blackboard& blackboard, bool updatedLastFrame) override;
        };

    }
}
