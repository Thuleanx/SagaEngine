#pragma once
#include "Engine/Entity/entity.h"
#include "Engine/_Core/logger.h"
#include <memory>
#include <map>
#include <optional>
#include <any>
#include <string>

namespace Saga {
    class GameWorld;

    class Blackboard {
        private:
            std::map<std::string, std::any> board;
        public:
            template <typename T>
                inline std::optional<T> get(const std::string &id) {
                    try {
                        if (!board.count(id)) 
                            return {};
                        return std::any_cast<T>(board.at(id));
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

            Entity entity;
            std::shared_ptr<Saga::GameWorld> world;
    };
}
