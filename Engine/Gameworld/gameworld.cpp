#include "gameworld.h"
#include "componentContainer.h"
#include "../_Core/asserts.h"

namespace Saga {

GameWorld::GameWorld() {

}

GameWorld::~GameWorld() {

}

Entity GameWorld::createEntity() {
	if (entity_cnt < 0)
		SWARN("entity_cnt of a game world is %d < 0. This should not happen since entity_cnt starts at 0 and only increments. Check if an inherited object has changed this value.", entity_cnt);
	entitySignatures[(Entity) entity_cnt] = Signature(0);
	return (Entity) (entity_cnt++);
}

void GameWorld::destroyEntity(Entity entity) {
	for (auto & [key, container] : componentMap) 
		container->onEntityDestroyed(entity);
}

}
