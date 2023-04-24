#include "behaviourtree.h"
#include <memory>

namespace Saga {

    void BehaviourTree::update(float deltaTime, Blackboard& blackboard) {
        if (root) {
            root->update(deltaTime, blackboard, !firstFrame);
            firstFrame = false;
        }
    }

    void BehaviourTree::setRoot(std::shared_ptr<Node> rootNode) {
        firstFrame = true;
        root = rootNode;
    }
}

