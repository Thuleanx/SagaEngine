#include "behaviourtree.h"
#include <memory>

namespace Saga {

    void BehaviourTree::update(Blackboard& blackboard) {
        if (root) {
            root->update(blackboard, !firstFrame);
            firstFrame = false;
        }
    }

    void BehaviourTree::setRoot(std::shared_ptr<Node> rootNode) {
        firstFrame = true;
        root = rootNode;
    }
}

