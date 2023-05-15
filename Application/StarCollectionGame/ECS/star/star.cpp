#include "star.h"

namespace Star::Systems {

void starCollect(std::shared_ptr<Saga::GameWorld> world, Saga::Entity entity, Saga::Entity other) {
    STRACE("call once");
    world->destroyEntity(entity);
}

}
