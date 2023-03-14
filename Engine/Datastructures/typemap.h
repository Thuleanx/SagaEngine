#pragma once

#include <atomic>
#include <unordered_map>

// maps types to values
namespace Saga {
	/**
	 * @brief Map that has Types as key.
	 * 
	 * @tparam DataType the type of the value of the map.
	 */
	template <class DataType>
	class TypeMap {
		using Map = std::unordered_map<int, DataType>;
	public:
		typedef typename Map::iterator iterator;
		typedef typename Map::const_iterator const_iterator;
		typedef typename Map::value_type value_type;

		const_iterator begin() const { return map.begin(); }
		const_iterator end() const { return map.end(); }
		iterator begin() { return map.begin(); }
		iterator end() { return map.end(); }

		template <class KeyType>
		iterator find() { return map.find(getTypeId<KeyType>()); }

		template <class KeyType>
		const_iterator find() const { return map.find(getTypeId<KeyType>()); }

		template <class KeyType>
		bool hasKey() { return find<KeyType>() != end(); }

		template <class KeyType>
		void put(DataType &&data) {
			map[getTypeId<KeyType>()] = std::forward<DataType>(data);
		}

		template <class KeyType>
		inline int getTypeId() {
            static int id = LastTypeId++;
			return id;
		}

		int size() { return map.size(); }

	private:
		static std::atomic_int LastTypeId;
		Map map;
	};

	template <class ValueType>
	std::atomic_int TypeMap<ValueType>::LastTypeId(0);
}
