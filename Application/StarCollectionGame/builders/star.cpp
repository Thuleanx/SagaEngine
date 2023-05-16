#include "star.h"
#include "Application/StarCollectionGame/ECS/star/star.h"
#include "Application/StarCollectionGame/config.h"
#include "Engine/Components/Particles/particleCollection.h"
#include "Engine/Components/Particles/particleEmitter.h"
#include "Engine/Components/collider.h"
#include "Engine/Components/material.h"
#include "Engine/Components/mesh.h"
#include "Engine/Components/transform.h"
#include "Engine/Systems/events.h"

namespace Star {

Saga::Entity createStar(std::shared_ptr<Saga::GameWorld> world, glm::vec3 pos) {
    Saga::Entity entity = world->createEntity();
    Saga::Entity effect = world->createEntity();

    world->emplace<Saga::Mesh>(entity, "Resources/Meshes/star.obj");
    /* world->emplace<Saga::Mesh>(entity, Saga::Mesh::StandardType::Sphere); */
    glm::vec3 starColor = palette.getColor(starColorIndex);

    world->emplace<Saga::Material>(entity, starColor)->material
        ->setEmission(starColor * 1.0f);
    world->emplace<Saga::Transform>(entity)->transform->setPos(pos)->setScale(0.5);

    world->emplace<Saga::Collider>(entity);
    world->emplace<Saga::CylinderCollider>(entity, 1, 0.5); // height, radius

    world->emplace<Star::Comet>(entity, Comet {
        .initialPos = pos,
        .rotationSpeed = 3,
        .boppingSpeed = 2,
        .boppingDistance = 1,
        .effect = effect
    });
    world->getSystems().addEventSystem(Saga::EngineEvents::OnCollision, entity,
        Saga::System<Saga::Entity, Saga::Entity>(Star::Systems::starCollect));

    GraphicsEngine::Global::graphics.addShader("starCollection",
    {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/particles/vertex.vert", "Resources/Shaders/particles/particleTextured.frag"});

    std::shared_ptr<GraphicsEngine::Shader> shader = GraphicsEngine::Global::graphics.getShader("starCollection");

    std::shared_ptr<GraphicsEngine::Texture> starTexture = std::make_shared<GraphicsEngine::Texture>(
        "Resources/Images/particles/starOutline.png");

    world->emplace<Saga::ParticleCollection>(effect,
            100,
            Saga::ParticleCollection::BlendMode::ADDITIVE,
            shader, starTexture);

    Saga::ParticleEmitter emitter = Saga::ParticleEmitter();
    emitter.burst = 30;
    emitter.emissionRate = 0;
    emitter.particleTemplate = Saga::ParticleTemplate {
        .position = glm::vec3(0,0,0),
        .velocity = glm::vec3(0, 2, 0),
        .velocityRandomness = glm::vec3(1, 1, 1) * 40.f,
        .gravity = 1,
        .color = glm::vec4(starColor,1),
        .size = 1,
        .sizeVariation = 0.5,
        .rotation = 0.0f,
        .lifetime = 5.0f
    };

    world->emplace<Saga::ParticleEmitter>(effect, emitter);
    world->emplace<Saga::Transform>(effect);


    return entity;
}

}
