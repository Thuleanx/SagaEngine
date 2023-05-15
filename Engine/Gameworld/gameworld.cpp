#include "gameworld.h"
#include "componentContainer.h"
#include "../_Core/asserts.h"

namespace Saga {

GameWorld::GameWorld() {

}

GameWorld::~GameWorld() {

}

// we can do this since we start entity_cnt at 1
Entity GameWorld::getMasterEntity() { return (Entity) 0; }

Entity GameWorld::createEntity() {
	if (entity_cnt < 0)
		SWARN("entity_cnt of a game world is %d < 0. This should not happen since entity_cnt starts at 0 and only increments. Check if an inherited object has changed this value.", entity_cnt);
	entitySignatures[(Entity) entity_cnt] = Signature(0);
	return (Entity) (entity_cnt++);
}

void GameWorld::destroyEntity(Entity entity) {
    entitiesToDestroy.insert(entity);
}

void GameWorld::entityCleanup() {
    for (Entity entity : entitiesToDestroy) {
        for (auto & [key, container] : componentMap) 
            container->onEntityDestroyed(entity);
        for (auto & [key, group] : componentGroups)
            group->removeEntity(entity);
        systemManager.onEntityDestroyed(entity);
    }
    entitiesToDestroy.clear();
}

}
