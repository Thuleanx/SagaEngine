#pragma once

#include <atomic>
#include <typeinfo>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <memory>
#include "../_Core/logger.h"

// maps event (practically ints) to functions
namespace Saga {
	/**
	 * @brief Generic callback used for events.
	 */
	class ICallback {
		public:
			virtual ~ICallback() = default;
	};

	/**
	 * @brief A wrapper around a shared_ptr to a function.
	 * 
	 * @tparam DataType the types of data the function accepts.
	 */
	template <typename ...DataType>
	class Callback : public ICallback {
    public:
		Callback(std::shared_ptr<std::function<void(DataType...)>> func) : func(func) {}

		/**
		 * @brief Call the function this stores.
		 * 
		 * @param args the parameters to the function.
		 */
        void evoke(DataType... args) { 
            if (!func) {
                SERROR("trying to evoke a null pointer");
            } else (*func)(args...); 
        }

	private:
		std::shared_ptr<std::function<void(DataType...)>> func;
	};

	/**
	 * @brief A mapping between events (which are ints under the hood) and callbacks.
     * @ingroup datastructures
	 */
	class EventMap {
	public: 
		enum Id: uint64_t {};
	private:
		using Map = std::unordered_map<int, std::unordered_map<Id, std::shared_ptr<ICallback>>>;
	public:
		typedef typename Map::iterator iterator;
		typedef typename Map::const_iterator const_iterator;
		typedef typename Map::value_type value_type;

		const_iterator begin() const { return map.begin(); }
		const_iterator end() const { return map.end(); }
		iterator begin() { return map.begin(); }
		iterator end() { return map.end(); }

		template <typename Event>
		iterator find(Event event) { return map.find((int) event); }

		template <typename Event>
		const_iterator find(Event event) const { return map.find((int) event); }

		/**
		 * @brief Add a listener to a specific event
		 * 
		 * @tparam Event anything castable to an int.
		 * @tparam DataType the type of data the callback function accepts.
		 * @param event 
		 * @param callbackFunc the callback that happens when this event is triggered.
		 * @return Id the id of the listener. This can be use to remove the listener later.
		 */
		template <typename Event, typename ...DataType>
		Id addListener(Event event, std::shared_ptr<std::function<void(DataType...)>> callbackFunc) {
			const Id id = Id(++id_value());

			map[(int) event][id] = std::make_shared<Callback<DataType...>>(callbackFunc);
			return id;
		}

		/**
		 * @brief Remove a listener from an event, by their id.
		 * 
		 * @tparam Event anything castable to an int.
		 * @param event 
		 * @param id 
		 */
		template <typename Event>
		void removeListener(Event event, Id id) {
			if (map.count((int) event)) {
				int removeCnt = map[(int) event].erase(id);
				if (!removeCnt) SWARN("Trying to remove listener %d from event %d, but the event does not have this listener.", id, event);
			} else 
				SWARN("Trying to remove a listener from event %d, but the event does not exist.", event);
		}

		/**
		 * @brief Invoke an event. This calls all listeners attached to that event, with the specified parameters.
		 * 
		 * @tparam Event anything castable to an int.
		 * @tparam DataType 
		 * @param event 
		 * @param args the parameters used in calling the listeners.
		 */
		template <typename Event, typename ...DataType>
		void invoke(Event event, DataType... args) {
            if (map.count((int) event))
                for (auto& [key, callback] : map[(int) event]) {
                    auto ptr = std::dynamic_pointer_cast<Callback<DataType...>>(callback);
                    ptr->evoke(args...);
                }
		}

        /**
         * @brief Remove an event, as well as any listeners attached to that event.
         *
         * @tparam Event anything castable to an int.
         * @param event to remove.
         */
        template <typename Event>
        void removeEvent(Event event) {
            map.erase((int) event);
        }

	private:
		Map map;
		static auto id_value()
			-> uint64_t & {
			static uint64_t the_id;
			return the_id;
		}
	};
}
