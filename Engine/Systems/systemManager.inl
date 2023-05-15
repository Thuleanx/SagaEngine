#pragma once
#include "systemManager.h"
#include "../_Core/asserts.h"
#include "../_Core/logger.h"

namespace Saga {
	template <typename ...DataType>
    EventMap::Id SystemManager::addStagedSystem(System<DataType...> system, Stage stage) {
        return stagedSystemsMap.addListener(stage, std::make_shared<System<DataType...>>(system)); }

	template <typename Event, typename ...DataType>
	EventMap::Id SystemManager::addEventSystem(Event event, System<DataType...> system) {
        return eventSystemsMap[(int) event].addListener((Entity) -1, std::make_shared<System<DataType...>>(system)); // ensures no message events gets triggered
	}

	template <typename Event>
	void SystemManager::removeEventSystem(Event event, EventMap::Id id) {
		int ev = (int) event;
		if (!eventSystemsMap.count(ev)) {
			SWARN("Attempt to remove a broadcast-type event system from event %s, which does not exist.", event);
			return;
		}
		eventSystemsMap[ev].removeListener((Entity) -1, id); 
	}

	
	template <typename Event, typename ...DataType>
	EventMap::Id SystemManager::addEventSystem(Event event, Saga::Entity entity, 
            std::shared_ptr<System<Saga::Entity, DataType...>> system) {
		return eventSystemsMap[(int) event].addListener(entity, system);
	}

	template <typename Event>
	void SystemManager::removeEventSystem(Event event, Saga::Entity entity, EventMap::Id id) {
		int ev = (int) event;
		if (!eventSystemsMap.count(ev)) {
			SWARN("Attempt to remove a delivery-type event system from event %s, which does not exist.", event);
			return;
		}
		eventSystemsMap[ev].removeListener(entity, id);
	}
}
