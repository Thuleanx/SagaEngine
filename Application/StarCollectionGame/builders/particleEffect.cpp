#include "Engine/Components/Particles/particleCollection.h"
#include "Engine/Components/Particles/particleEmitter.h"
#include "Graphics/GLWrappers/texture.h"
#include "Graphics/global.h"
#include "particleEffects.h"

namespace Star {

Saga::Entity createStarCollectionParticle(std::shared_ptr<Saga::GameWorld> world, glm::vec3 pos) {
    Saga::Entity ent = world->createEntity();

    GraphicsEngine::Global::graphics.addShader("starCollection",
    {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
    {"Resources/Shaders/particles/vertex.vert", "Resources/Shaders/particles/particleTextured.frag"});

    std::shared_ptr<GraphicsEngine::Shader> shader = GraphicsEngine::Global::graphics.getShader("starCollection");

    std::shared_ptr<GraphicsEngine::Texture> starTexture = std::make_shared<GraphicsEngine::Texture>(
        "Resources/Images/particles/starOutline");

    world->emplace<Saga::ParticleCollection>(ent,
            1000,
            Saga::ParticleCollection::BlendMode::ADDITIVE,
            shader, starTexture);

    world->emplace<Saga::ParticleEmitter>(ent);

    return ent;
}

}
