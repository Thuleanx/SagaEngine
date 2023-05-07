#include "circleBlur.h"
#include "Engine/Graphics/separableKernel.h"
#include "Engine/_Core/logger.h"
#include "Graphics/global.h"

namespace Saga::Graphics {
    CircleBlur::CircleBlur(int width, int height,
        std::shared_ptr<GraphicsEngine::Texture> texture0,
        std::shared_ptr<GraphicsEngine::Texture> texture1, float size) : 
        SeparableKernel(width, height, texture0, texture1), size(size) {

        if (size < 0) {
            SERROR("Size of max filter must be non negative.");
        }

        constructShader();
        GraphicsEngine::Global::graphics.bindShader(getShader());
        initializeShaderParams();
    }

    CircleBlur::~CircleBlur() {
        GraphicsEngine::Global::graphics.removeShader(getShader());
    }

    void CircleBlur::setSize(float size) {
        this->size = size;
        // bind shader and set the size.
        GraphicsEngine::Global::graphics.bindShader(getShader());
        GraphicsEngine::Global::graphics.getActiveShader()->setFloat("blurRadius", size);
    }

    std::string CircleBlur::getShader() { return shaderID + std::to_string(uid); }

    void CircleBlur::constructShader() {
        using namespace GraphicsEngine::Global;
        graphics.addShader(getShader(), {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
        {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/standard/circleBlur.frag"});
    }

    void CircleBlur::initializeShaderParams() {
        using namespace GraphicsEngine::Global;
        auto shader = graphics.getActiveShader();
        shader->setFloat("blurRadius", size);
        shader->setSampler("MainTex", 0);
    }
}
