#pragma once

#include "Engine/Components/camera.h"
#include "Graphics/GLWrappers/framebuffer.h"
#include "Graphics/GLWrappers/texture.h"
#include <memory>

namespace Saga::Graphics {
/**
 * @brief A Fog pass used to apply screen space fog onto a texture.
 */
class Fog {
public:
    /**
     * @Create a Fog pass used to apply screen space fog.
     *
     * @param width the texture's width.
     * @param height the texture's height.
     * @param resultTex where to store the result of the Fog pass.
     * @param depthTex the depth texture to sample from.
     */
    Fog(int width, int height, std::shared_ptr<GraphicsEngine::Texture> resultTex);
    ~Fog();

    /**
     * @brief Run the fog pass with the given depth values.
     *
     * @param camera used to supply the internal shader with values for it to be able to convert perspective depth
     *  to usable depth values.
     * @param mainTex the main color texture to use.
     * @param depthTex the depth texture.
     * @param fogColor determines the color of the most foggy area of the screen.
     * @param fogDensity determines how dense the fog gets with distance.
     */
    void applyFog(Saga::Camera& camera, 
        std::shared_ptr<GraphicsEngine::Texture> mainTex, std::shared_ptr<GraphicsEngine::Texture> depthTex,
        glm::vec3 fogColor, float fogDensity);

private:
    const std::string fogShaderId = "fog"; //!< id of the fog shader in the graphic engine's eye.

    const int uid; //!< used to create new fbos.
    int width, //!< width in pixels of the resulting texture
        height; //!< height in pixels of the resulting texture
    std::string fbo; //!< name of the framebuffer used for drawing fog.

    static int newUID; //!< use to assign new id to newly created fog objects.

    /**
     * @brief Load shader with important parameters for fog calculation.
     *
     * @param camera from which the shader extracts the near and far distances.
     * @param fogColor the color of the fog.
     * @param fogDensity the density of the fog.
     */
    void loadShaderWithParam(Saga::Camera& camera, glm::vec3 fogColor, float fogDensity);
};
}
