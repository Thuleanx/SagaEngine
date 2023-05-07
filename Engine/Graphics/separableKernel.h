#pragma once

#include "Graphics/GLWrappers/framebuffer.h"
#include "Graphics/GLWrappers/texture.h"
#include <memory>

namespace Saga::Graphics {

/**
 * @brief Classic separable kernel (i.e. gaussian blur or max operator).
 */
class SeparableKernel {
public:
    /**
     * @brief Create a separable kernel object that can apply an effect to a texture of specified width and height.
     *
     * @param width the texture's width.
     * @param height the texture's height.
     * @param texture0 the texture to apply the effect to, and where the result will be stored.
     * @param texture1 another texture with the same parameter with texture0, 
     *  used as an intermediary texture in applying the effect.
     */
    SeparableKernel(int width, int height,
        std::shared_ptr<GraphicsEngine::Texture> texture0,
        std::shared_ptr<GraphicsEngine::Texture> texture1);
    /**
     * @brief Destructor for the separable object.
     */
    ~SeparableKernel();

    /**
     * @brief Initialize the effect, which loads the shaders used for this effect.
     */
    void init();

    /**
     * @brief Run the kernel over a specified number of iterations.
     *
     * @param iterations number of times to apply the kernel. This must strictly be non negative.
     */
    void apply(int iterations);

private:
    int width; //!< width in pixels of the texture to apply the effect.
    int height; //!< height in pixel of the texture to apply the effect.
    std::string fbo[2]; //!< fbo objects used in the effect. One for the horizontal and one vertical pass.
    std::shared_ptr<GraphicsEngine::Texture> textures[2]; //!< textures used in the effect. textures[0] is where the result will be.

    static int newUID; //!< used purely to assign new ids to new objects and making sure they are unique.

protected:
    const int uid; //!< id used to create new fbos. Unique for each object.
    /**
     * @brief Get the id of the shader used in this effect.
     *
     * @note this shader must have the parameter [_MainTex] as the main texture (since we are using blit), 
     * and an additional uniform called [horizontal] which determines if this shader is running the horizontal or vertical pass.
     *
     * @return std::string shader id used for the effect in the graphics engine.
     */
    virtual std::string getShader() = 0;

    /**
     * @brief Construct the shader for the separable kernel.
     */
    virtual void constructShader() = 0;

    /**
     * @brief Initialize starting parameters for the separable kernel.
     *
     * @pre shader has been bound.
     */
    virtual void initializeShaderParams() = 0;
};

}
