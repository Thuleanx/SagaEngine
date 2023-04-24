#pragma once

#include <vector>
#include <memory>
#include <map>
#include <any>
#include <optional>
#include "Engine/Components/BehaviourTree/blackboard.h"
#include "Engine/Entity/entity.h"
#include "Engine/_Core/logger.h"

namespace Saga {
    class GameWorld;

    class BehaviourTree {
    public:
        enum Status {
            SUCCESS, FAIL, RUNNING
        };

        class Node {
            public:
                virtual Status update(float seconds, Blackboard &blackboard, bool updatedLastFrame) = 0;
        };


        void update(float deltaTime, Blackboard& blackboard);

        template <typename T, typename ...Args>
        std::shared_ptr<T> makeRoot(Args &&... args) {
            static_assert(std::is_base_of<Node, T>::value, "Type assigned to root must be a Behaviour tree node");
            firstFrame = true;
            std::shared_ptr<T> newRootPtr = std::make_shared<T>(args...);
            root = newRootPtr;
            return newRootPtr;
        }

        void setRoot(std::shared_ptr<Node> rootNode);
    private:
        bool firstFrame = true;
        std::shared_ptr<Node> root;
    };
}
