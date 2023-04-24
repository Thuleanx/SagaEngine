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

    /**
     * @brief A blackboard used to store information. Formally a mapping from string to std::any.
     * This is used by behaviour trees to read, store, and communicate information.
     */
    class Blackboard {
        private:
            std::map<std::string, std::any> board; //!< maps string to raw pieces of information
        public:
            /**
             * @brief Retrieve a value from the map.
             *
             * @tparam T the type of the value inside the map.
             *
             * @param key a string key to the information in the map. This will be retrieved and reinterpreted
             *  (std::any_cast) to the requested data type.
             *
             * @return nothing if either the board does not have the key, or the type requested is incompatible.
             * @return T the data in the requested type, if possible.
             */
            template <typename T>
                inline std::optional<T> get(const std::string &key) {
                    try {
                        if (!board.count(key)) 
                            return {};
                        return std::any_cast<T>(board.at(key));
                    } catch (const std::bad_any_cast& e) {
                        std::string fmt = "Trying to retrieve item of id %s in blackboard. \
                                           Wanted type [%s] but the blackboards data is actually [%s]: \
                                           get error: %s";
                        SERROR(fmt.c_str(), key.c_str(), typeid(T).name(), board.at(key).type().name(), e.what());
                    }
                    return {};
                }

            /**
             * @brief Put a value in the map. This overrides whatever value that's inside, regardless of type.
             *
             * @tparam T the type of the value being put in. The user of this function need not explicitly specify this.
             *      if not specified, then it is defaulted to the type of value.
             *
             * @param key the string key into the map.
             * @param value the value to put into the blackboard.
             */
            template <class T>
                inline void put(const std::string &key, T value) { board[key] = value; }

            // Bits of information useful for a behaviour tree node to know, that doesn't go into the map 
            // because every node will need to use these.
            Entity entity; //!< the entity the blackboard is located on.
            std::shared_ptr<Saga::GameWorld> world; //!< the game world the blackboard is in.
            float time; //!< time elapsed since start of the program.
            float deltaTime; //!< time between this and last frame
    };
}
