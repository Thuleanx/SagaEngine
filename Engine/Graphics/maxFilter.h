#pragma once

#include "Engine/Graphics/separableKernel.h"
#include "Graphics/GLWrappers/framebuffer.h"
#include "Graphics/GLWrappers/texture.h"
#include <memory>

namespace Saga::Graphics {

/**
 * @brief Classic gaussian blur operator used to blur a texture.
 */
class MaxFilter : public SeparableKernel {
public:
    /**
     * @brief Create a max filter object that, for each pixel, centers a box around it and take the max value.
     * This is performed exclusively on textures with GL_RED format.
     *
     * @param width the texture's width.
     * @param height the texture's height.
     * @param texture0 the texture to blur, and where the result will be stored.
     * @param texture1 another texture with the same parameter with texture0, used as an intermediary texture in the blur.
     * @param size the size of the max window. Must be non negative.
     */
    MaxFilter(int width, int height,
        std::shared_ptr<GraphicsEngine::Texture> texture0,
        std::shared_ptr<GraphicsEngine::Texture> texture1, int size);
    /**
     * @brief Destructor for the gaussian blur.
     */
    ~MaxFilter();

    /**
     * @brief Set the kernel size.
     *
     * @param size the desired size, in pixels.
     */
    void setSize(int size);

private:
    const std::string shaderID = "maxFilter"; //!< Id of the blur shader in the graphics engine's eye.
    int size; //!< size of the filter.

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
