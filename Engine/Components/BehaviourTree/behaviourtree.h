#pragma once

#include <vector>
#include <memory>
#include <map>
#include <any>
#include <optional>
#include "Engine/_Core/logger.h"

namespace Saga {
    class BehaviourTree {
    public:
        enum Status {
            SUCCESS, FAIL, RUNNING
        };

        class Blackboard {
        private:
            std::map<std::string, std::any> board;
        public:
            template <typename T>
            inline std::optional<T> get(const std::string &id) {
                try {
                    if (!board.count(id)) 
                        return {};
                    return std::any_cast<T>(id);
                } catch (const std::bad_any_cast& e) {
                    std::string fmt = "Trying to retrieve item of id %s in blackboard. \
                                       Wanted type [%s] but the blackboards data is actually [%s]: \
                                       get error: %s";
                    SERROR(fmt.c_str(), id.c_str(), typeid(T).name(), board.at(id).type().name(), e.what());
                }
                return {};
            }

            template <class T>
            inline void put(const std::string &id, T value) { board[id] = value; }
        };

        class BehaviourTreeNode {
            public:
                virtual Status update(float seconds, Blackboard &blackboard, bool updatedLastFrame) = 0;
        };



        void update(float deltaTime, Blackboard& blackboard);
    private:
        std::unique_ptr<BehaviourTreeNode> root;
    };
}
