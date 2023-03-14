#pragma once

namespace Saga {
	const int ENGINE_EVENT_START_VALUE = 10000;
	/**
	 * @brief Define specific engine events. These events start at the value ENGINE_EVENT_START_VALUE
	 * \see ENGINE_EVENT_START_VALUE.
	 */
	enum class EngineEvents {
		OnCollision = 1000
	};
}