#include "maxFilter.h"
#include "Engine/Graphics/separableKernel.h"
#include "Engine/_Core/logger.h"
#include "Graphics/global.h"

namespace Saga::Graphics {
    MaxFilter::MaxFilter(int width, int height,
        std::shared_ptr<GraphicsEngine::Texture> texture0,
        std::shared_ptr<GraphicsEngine::Texture> texture1, int size) : 
        SeparableKernel(width, height, texture0, texture1), size(size) {

        if (size < 0) {
            SERROR("Size of max filter must be non negative.");
        }

        constructShader();
        GraphicsEngine::Global::graphics.bindShader(getShader());
        initializeShaderParams();
    }

    MaxFilter::~MaxFilter() {
        GraphicsEngine::Global::graphics.removeShader(getShader());
    }

    void MaxFilter::setSize(int size) {
        this->size = size;
        // bind shader and set the size.
        GraphicsEngine::Global::graphics.bindShader(getShader());
        GraphicsEngine::Global::graphics.getActiveShader()->setInt("kernelSz", size);
    }

    std::string MaxFilter::getShader() { return shaderID + std::to_string(uid); }

    void MaxFilter::constructShader() {
        using namespace GraphicsEngine::Global;
        graphics.addShader(getShader(), {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER},
        {"Resources/Shaders/postprocessing.vert", "Resources/Shaders/standard/max.frag"});
    }

    void MaxFilter::initializeShaderParams() {
        using namespace GraphicsEngine::Global;
        auto shader = graphics.getActiveShader();
        shader->setInt("kernelSz", size);
        shader->setSampler("MainTex", 0);
    }
}
