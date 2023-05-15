#include "star.h"

namespace Star::Systems {

void starCollect(std::shared_ptr<Saga::GameWorld> world, Saga::Entity entity, Saga::Systems::Collision collision) {
    world->destroyEntity(entity);
}

}
