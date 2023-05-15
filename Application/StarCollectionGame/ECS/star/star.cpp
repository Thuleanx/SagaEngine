#include "star.h"
#include "Application/StarCollectionGame/ECS/player/player.h"
#include "Engine/Components/transform.h"

namespace Star::Systems {

void starCollect(std::shared_ptr<Saga::GameWorld> world, Saga::Entity entity, Saga::Entity other) {
    STRACE("What is happening");
    world->destroyEntity(entity);
}

void playerGrowth(std::shared_ptr<Saga::GameWorld> world, Saga::Entity player, Saga::Entity other) {
    if (!world->hasComponent<Comet>(other)) return;

    Player* playerInfo = world->getComponent<Player>(player);
    Saga::Transform* transform = world->getComponent<Saga::Transform>(player);
    Camera* camera = world->getComponent<Camera>(player);
    if (!playerInfo) return;
    if (!transform) return;

    playerInfo->numStarsCollected++;

    glm::vec3 prevScale = transform->transform->getScale();
    glm::vec3 currentScale = glm::vec3(playerInfo->sizeFactor());

    camera->distance *= std::sqrt(playerInfo->growthFactor);
    // push upward so we don't end up inside terrain
    transform->transform->setPos(transform->getPos() + (currentScale.y - prevScale.y) * glm::vec3(0,1,0));
    transform->transform->setScale(currentScale);

}

void animateStar(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time) {
    for (auto& [entity, comet, transform] : *world->viewGroup<Star::Comet, Saga::Transform>()) {
        glm::vec3 position = comet->initialPos + std::sin(comet->boppingSpeed * time) * comet->boppingDistance * glm::vec3(0,1,0);
        transform->transform->setPos(position);
        transform->transform->rotate(deltaTime * comet->rotationSpeed, glm::vec3(0,1,0));
    }
}

}
