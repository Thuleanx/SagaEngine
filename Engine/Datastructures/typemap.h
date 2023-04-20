#pragma once

#include <atomic>
#include <unordered_map>

// maps types to values
namespace Saga {
	/**
	 * @brief Map that has Types as key.
	 * 
	 * @tparam DataType the type of the value of the map.
     * @ingroup datastructures
	 */
	template <class DataType>
	class TypeMap {
		using Map = std::unordered_map<int, DataType>;
	public:
		typedef typename Map::iterator iterator;
		typedef typename Map::const_iterator const_iterator;
		typedef typename Map::value_type value_type;

        /**
         * @brief Return a const iterator to the beginning of this typemap.
         *
         * @return const_iterator a const iterator pointing to the beginning of the 
         * typemap.
         */
		const_iterator begin() const { return map.begin(); }

        /**
         * @brief Return a const iterator to the end of this typemap.
         *
         * @return const_iterator a const iterator pointing to the end of the 
         * typemap.
         */
		const_iterator end() const { return map.end(); }

        /**
         * @brief Return an iterator to the beginning of this typemap.
         *
         * @return iterator an iterator pointing to the beginning of the 
         * typemap.
         */
		iterator begin() { return map.begin(); }

        /**
         * @brief Return an iterator to the end of this typemap.
         *
         * @return iterator an iterator pointing to the end of the 
         * typemap.
         */
		iterator end() { return map.end(); }

        /**
         * @brief Find a type inside this map.
         *
         * @tparam KeyType the type to find.
         * @return an iterator pointing to the key-value pair in the map with key = keyType, or 
         * the end iterator otherwise.
         */
		template <class KeyType>
		iterator find() { return map.find(getTypeId<KeyType>()); }

        /**
         * @brief Find a type inside this map.
         *
         * @tparam KeyType the type to find.
         * @return a const iterator pointing to the key-value pair in the map with key = keyType, or 
         * the end iterator otherwise.
         */
		template <class KeyType>
		const_iterator find() const { return map.find(getTypeId<KeyType>()); }

        /**
         * @brief Determine if the map already maps a type to some value.
         *
         * @tparam KeyType the type to search for.
         * @return true if the type is inside the map.
         * @return false otherwise.
         */
		template <class KeyType>
		bool hasKey() { return find<KeyType>() != end(); }


        /**
         * @brief Place an item inside the map. If the map already contains this
         * KeyType as a key, then replace the mapping to data. Otherwise, create 
         * a mapping between KeyType and data.
         *
         * @tparam KeyType the key used in the mapping.
         * @param data that we map KeyType to.
         */
		template <class KeyType>
		void put(DataType &&data) {
			map[getTypeId<KeyType>()] = std::forward<DataType>(data);
		}

        /**
         * @brief Generate a unique ID for a type. Any call to this with the same
         * type will return the same value.
         *
         * @tparam KeyType the type.
         */
		template <class KeyType>
		inline int getTypeId() {
            static int id = LastTypeId++;
			return id;
		}

        /**
         * @brief Determine the number of mappings this map has.
         *
         * @return int the number of mappings the map has.
         */
		int size() { return map.size(); }

	private:
		static std::atomic_int LastTypeId; //!< Used for assigning integer id to certain types.
		Map map; //!< The internal map.
	};

	template <class ValueType>
	std::atomic_int TypeMap<ValueType>::LastTypeId(0); //!< We start lastTypeId out at 0
}
