#pragma once

#include "Engine/Components/camera.h"
#include "Graphics/GLWrappers/vao.h"
#include "Graphics/cubemap.h"
#include <string>

namespace Saga::Graphics {

/**
 * @brief Represents a skybox, which is essentially a series of 6 images rendered 
 * at the extremes of the view window.
 */
class Skybox {
public:
    /**
     * @brief Create a new skybox object. This also loads the shader the skybox uses.
     *
     * @param skyboxPath should be a path to a directory containing the skybox.
     *  the 6 files of the skyboxes must be pngs of the names:
     *  right, left, up, down, front, back.
     *  @param fileextension the extension of the skybox files (i.e. "png").
     */
    Skybox(const std::string &skyboxPath, const std::string &skyboxExtension);

    /**
     * @brief Destroy the skybox
     */
    ~Skybox();

    /**
     * @brief Draw the skybox onto the screen.
     */
    void draw(Saga::Camera& camera);
private:
    const std::string shaderId = "skybox"; //!< id for the skybox shader.
    std::shared_ptr<GraphicsEngine::CubeMap> skyTexture; //!< contains the 6 faces of the sky.
 
    std::shared_ptr<GraphicsEngine::VAO> vao; //!< vertex array for the sky.
    std::shared_ptr<GraphicsEngine::VBO> vbo; //!< vertex buffer for the sky.
};
}
