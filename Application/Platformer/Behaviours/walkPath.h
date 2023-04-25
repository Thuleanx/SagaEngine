#pragma once

#include "Engine/Components/BehaviourTree/tasknodes.h"

namespace Platformer {
    class WalkPath : public Saga::BehaviourTreeNodes::Task {
        public:
            WalkPath(const std::string& pathKey, const std::string& speedKey);

            virtual Saga::BehaviourTree::Status update(Saga::Blackboard& blackboard, bool executedLastFrame) override;
        private:
            const std::string pathKey;
            const std::string speedKey;
    };
}
