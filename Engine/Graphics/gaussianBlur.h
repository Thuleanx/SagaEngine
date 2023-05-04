#pragma once

#include "Graphics/GLWrappers/framebuffer.h"
#include "Graphics/GLWrappers/texture.h"
#include <memory>

namespace Saga::Graphics {

/**
 * @brief Classic gaussian blur operator used to blur a texture.
 */
class GaussianBlur {
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
     * @brief Run the gaussian blur over a specified number of iterations.
     *
     * @param iterations number of times to run the gaussian blur. This must strictly be non negative.
     * @param quad the full screen quad used to blit the shader onto the textures.
     */
    void blur(int iterations);

private:
    const std::string blurShaderId = "gaussianBlur"; //!< Id of the blur shader in the graphics engine's eye.
    const int KERNEL_SIZE = 16; //!< size of the gaussian blur kernel. In practice, this should be at least three times the sigma.

    const int uid; //!< id used to create new fbos
    float sigma; //!< standard deviation of the blur.
    int width; //!< width in pixels of the texture to blur.
    int height; //!< height in pixel of the texture to blur.
    std::string fbo[2]; //!< fbo objects used in the blurring. One for the horizontal and one vertical pass.
    std::shared_ptr<GraphicsEngine::Texture> textures[2]; //!< textures used in the gaussian blur.

    static int newUID;

    /**
     * @brief Load the blur shader with neccessary parameters, such as the values of the kernel.
     */
    void loadShaderWithParam();
};

}
