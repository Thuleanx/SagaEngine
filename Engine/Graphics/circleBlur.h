#pragma once

#include "Engine/Graphics/separableKernel.h"
#include "Graphics/GLWrappers/framebuffer.h"
#include "Graphics/GLWrappers/texture.h"
#include <memory>

namespace Saga::Graphics {

/**
 * @brief Classic circle blur operator used to blur a texture.
 */
class CircleBlur : public SeparableKernel {
public:
    /**
     * @brief Create a circle blur filter object that blurs a texutre.
     *
     * @param width the texture's width.
     * @param height the texture's height.
     * @param texture0 the texture to blur, and where the result will be stored.
     * @param texture1 another texture with the same parameter with texture0, used as an intermediary texture in the blur.
     * @param size the size of the circle blur.
     */
    CircleBlur(int width, int height,
        std::shared_ptr<GraphicsEngine::Texture> texture0,
        std::shared_ptr<GraphicsEngine::Texture> texture1, float size = 1);
    /**
     * @brief Destructor for the gaussian blur.
     */
    ~CircleBlur();

    /**
     * @brief Set the kernel size.
     *
     * @param size the desired size, in pixels.
     */
    void setSize(float size);

private:
    const std::string shaderID = "circleBlurFilter"; //!< Id of the blur shader in the graphics engine's eye.
    float size; //!< size of the filter.
protected:
    /**
     * @brief Find the name of the blur shader.
     *
     * @return std::string the id of the shader in the graphics engine, which is GaussianBlur::blurShaderId.
     */
    std::string getShader() override;

    /**
     * @brief Construct the shader for blurring.
     */
    void constructShader() override;

    /**
     * @brief Loads the kernel size, sigma, and various weights for the gaussian blur shader.
     *
     * @pre blur shader has been bound.
     */
    void initializeShaderParams() override;
};

}
