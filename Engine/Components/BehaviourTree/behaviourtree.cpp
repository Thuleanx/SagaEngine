#include "behaviourtree.h"
#include <memory>

namespace Saga {

    void BehaviourTree::update(float deltaTime, Blackboard& blackboard) {
        if (root) root->update(deltaTime, blackboard, true);
    }


}

