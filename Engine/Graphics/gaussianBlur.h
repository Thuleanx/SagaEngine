#pragma once

#include "Engine/Graphics/separableKernel.h"
#include "Graphics/GLWrappers/framebuffer.h"
#include "Graphics/GLWrappers/texture.h"
#include <memory>

namespace Saga::Graphics {

/**
 * @brief Classic gaussian blur operator used to blur a texture.
 */
class GaussianBlur : public SeparableKernel {
public:
    /**
     * @brief Create a gaussian blur object that blurs a texture of specified width and height.
     *
     * @param width the texture's width.
     * @param height the texture's height.
     * @param texture0 the texture to blur, and where the result will be stored.
     * @param texture1 another texture with the same parameter with texture0, used as an intermediary texture in the blur.
     * @param sigma the standard deviation of the gaussian operator. The higher, the more blurred the image will be.
     *
     * @note sigma should be supplied so that 3*sigma < KERNEL_SIZE. Otherwise, you will get cut-off in the blur.
     */
    GaussianBlur(int width, int height,
        std::shared_ptr<GraphicsEngine::Texture> texture0,
        std::shared_ptr<GraphicsEngine::Texture> texture1, float sigma = 3);
    /**
     * @brief Destructor for the gaussian blur.
     */
    ~GaussianBlur();

    /**
     * @brief Set radius for the blur.
     *
     * @param radius a scale to the blur's radius.
     */
    void setRadius(float radius);

private:
    const std::string blurShaderId = "gaussianBlur"; //!< Id of the blur shader in the graphics engine's eye.
    const int KERNEL_SIZE = 16; //!< size of the gaussian blur kernel. In practice, this should be at least three times the sigma.

    float sigma; //!< standard deviation of the blur.

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
