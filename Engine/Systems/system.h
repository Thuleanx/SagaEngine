#pragma once
#include <functional>
#include <memory>

namespace Saga {
	class GameWorld;

	/**
	 * @brief System is a function that accepts a shared_ptr to a GameWorld, as well as extra variables.
	 * 
	 * @tparam DataType the types of extra variables that a system accept
	 */
	template <typename ...DataType>
	using System = std::function<void(std::shared_ptr<GameWorld>, DataType...)>;
}
