#include "skybox.h"
#include "Graphics/cubemap.h"
#include "Graphics/global.h"

namespace Saga::Graphics {

namespace {

std::vector<std::string> faces = {
    "right",
    "left",
    "top",
    "bottom",
    "front",
    "back"
};

std::vector<GLenum> facesType = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

}

Skybox::Skybox(const std::string &skyboxPath, const std::string& fileExtension) {
    std::vector<std::string> filenames;
    for (int i = 0; i < 6; i++) filenames.push_back(skyboxPath + faces[i] + "." + fileExtension);

    skyTexture = std::make_shared<GraphicsEngine::CubeMap>(
        filenames,
        facesType
    );

    int verts = 6*6;
    std::vector<float> pos(3*verts);
    for (int i = 0; i < verts*3; i++)
        pos[i] = skyboxVertices[i];

    vbo = std::make_shared<GraphicsEngine::VBO>(pos);
    vao = std::make_shared<GraphicsEngine::VAO>(vbo, GraphicsEngine::VAOAttrib::POS);

    using namespace GraphicsEngine::Global;
    graphics.addShader(shaderId,
    {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER}, {"Resources/Shaders/skybox/skybox.vert", "Resources/Shaders/skybox/skybox.frag"});

    SINFO("Skybox at directory %s.", skyboxPath.c_str());
}

Skybox::~Skybox() {
}

void Skybox::draw(Saga::Camera& camera) {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    using namespace GraphicsEngine::Global;
    graphics.bindShader(shaderId);

    glActiveTexture(GL_TEXTURE0);
    skyTexture->bind();

    graphics.getActiveShader()->setMat4("projection", camera.camera->getProjection());
    graphics.getActiveShader()->setMat4("view", glm::mat4(glm::mat3(camera.camera->getView())));
    graphics.getActiveShader()->setSampler("Skybox", 0);


    vao->bind();
    vao->draw();

    skyTexture->unbind();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

}
