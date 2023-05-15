#include "star.h"
#include "Application/StarCollectionGame/ECS/player/player.h"
#include "Engine/Components/collider.h"
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
    Saga::EllipsoidCollider* ellipsoidCollider = world->getComponent<Saga::EllipsoidCollider>(player);
    Saga::CylinderCollider* cylinderCollider = world->getComponent<Saga::CylinderCollider>(player);
    Camera* camera = world->getComponent<Camera>(player);
    Saga::Material* mat = world->getComponent<Saga::Material>(player);
    if (!playerInfo || !transform || !camera || !ellipsoidCollider || !cylinderCollider) return;
    if (playerInfo->starsCollected.count((int) other)) return;

    playerInfo->starsCollected.insert((int) other);

    playerInfo->growthValue = std::pow(playerInfo->growthFactor, playerInfo->starsCollected.size());

    glm::vec3 prevScale = transform->transform->getScale();
    glm::vec3 currentScale = glm::vec3(playerInfo->growthValue);

    camera->distance *= playerInfo->growthFactor;
    // push upward so we don't end up inside terrain
    transform->transform->setPos(transform->getPos() + (currentScale.y - prevScale.y) * glm::vec3(0,1,0));
    transform->transform->setScale(currentScale);

    ellipsoidCollider->radius = currentScale / 2.0f;
    cylinderCollider->radius = currentScale.x/2.0f;
    cylinderCollider->height = currentScale.y;

    mat->material->setEmission(playerInfo->glowValue());
}

void animateStar(std::shared_ptr<Saga::GameWorld> world, float deltaTime, float time) {
    for (auto& [entity, comet, transform] : *world->viewGroup<Star::Comet, Saga::Transform>()) {
        glm::vec3 position = comet->initialPos + std::sin(comet->boppingSpeed * time) * comet->boppingDistance * glm::vec3(0,1,0);
        transform->transform->setPos(position);
        transform->transform->rotate(deltaTime * comet->rotationSpeed, glm::vec3(0,1,0));
    }
}

}
