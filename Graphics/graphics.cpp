#include "graphics.h"
#include "Engine/_Core/logger.h"
#include "shapedata.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace GraphicsEngine {

Graphics::Graphics():
    m_textRenderer(std::make_shared<TextRenderer>())
{

}

Graphics::~Graphics(){

}

void Graphics::initializeGLEW(){
    glewExperimental = GL_TRUE;
    SINFO("GLEWInit status: %d", glewInit());
}

void Graphics::initialize(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    SINFO("initalize text renderer");
    m_textRenderer->initialize();

    SINFO("add standard shapes");
    addShape("quad", quadVertexBufferData, VAOAttrib::POS | VAOAttrib::NORM | VAOAttrib::UV);
    addShape("cube", cubeVertexBufferData, VAOAttrib::POS | VAOAttrib::NORM | VAOAttrib::UV);
    addShape("sphere", sphereVertexBufferData, VAOAttrib::POS | VAOAttrib::NORM | VAOAttrib::UV);
    addShape("cylinder", cylinderVertexBufferData, VAOAttrib::POS | VAOAttrib::NORM | VAOAttrib::UV);
    addShape("cone", coneVertexBufferData, VAOAttrib::POS | VAOAttrib::NORM | VAOAttrib::UV);

    addShader("phong", {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER}, {"Resources/Shaders/phong.vert", "Resources/Shaders/phong.frag"});
    addShader("text", {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER}, {"Resources/Shaders/text.vert", "Resources/Shaders/text.frag"});
    bindShader("phong");
    
    addMaterial("default", glm::vec3(1));

    SINFO("adding font: %s", "opensans");
    addFont("opensans", "Resources/Fonts/OpenSans.ttf");
    getFont("opensans");
}

void Graphics::clearScreen(GLbitfield mask){
    glClear(mask);
}

void Graphics::setClearColor(glm::vec3 clearColor){
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1);
}

void Graphics::setCameraData(std::shared_ptr<Camera> camera){
    m_active_shader->setCamera(camera);
}

void Graphics::addFramebuffer(std::string framebufferName, int width, int height) {
    m_framebuffers.insert({framebufferName, std::make_shared<Framebuffer>(width, height)});
}

void Graphics::removeFramebuffer(std::string framebufferName){
    m_framebuffers.erase(framebufferName);
}

std::shared_ptr<Framebuffer> Graphics::getFramebuffer(std::string framebufferName) {
    if (m_framebuffers.count(framebufferName))
        return m_framebuffers.at(framebufferName);
    return nullptr;
}

void Graphics::bindFramebuffer(std::string framebufferName) {
    if (m_framebuffers.count(framebufferName)) {
        m_framebuffers.at(framebufferName)->bind();
        m_active_framebuffer = m_framebuffers.at(framebufferName);
    } else
        SERROR("Cannot find Framebuffer with Name: %s", framebufferName.c_str());
}

void Graphics::bindDefaultFramebuffer() {
    m_active_framebuffer->unbind();
    m_active_framebuffer = nullptr;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Graphics::addShader(std::string shaderName, std::vector<GLenum> shaderTypes, std::vector<const char*> filepaths){
    m_shaders.insert({shaderName, std::make_shared<Shader>(shaderTypes, filepaths)});
}

void Graphics::removeShader(std::string shaderName){
    if(shaderName != "phong" && shaderName != "text"){
        m_shaders.erase(shaderName);
    }
}

std::shared_ptr<GraphicsEngine::Shader> Graphics::getActiveShader() {
    return m_active_shader;
}

std::shared_ptr<Shader> Graphics::getShader(std::string shaderName) {
	if (m_shaders.count(shaderName)) return m_shaders[shaderName];
	return std::shared_ptr<Shader>();
}

void Graphics::bindShader(std::string shaderName){
    if (!m_shaders.count(shaderName)) {
        SERROR("Shader of name %s not found", shaderName.c_str());
        return;
    }
    m_shaders.at(shaderName)->bind();
    m_active_shader = m_shaders.at(shaderName);
}

std::shared_ptr<Shape> Graphics::addShape(std::string shapeName, std::vector<float> data, VAOAttrib attribs){
    m_shapes.insert({shapeName, std::make_shared<Shape>(std::make_shared<VAO>(std::make_shared<VBO>(data), attribs))});
    return m_shapes.at(shapeName);
}
std::vector<glm::vec3> Graphics::addShape(std::string shapeName, std::string filepath){
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    std::vector<float> drawData;
    std::vector<glm::vec3> collisionData;

    for(size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            unsigned int fv = shapes[s].mesh.num_face_vertices[f];

            for(size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                // Add position
                drawData.push_back(attrib.vertices[3*idx.vertex_index]);
                drawData.push_back(attrib.vertices[3*idx.vertex_index + 1]);
                drawData.push_back(attrib.vertices[3*idx.vertex_index + 2]);
                // Add normal
                drawData.push_back(attrib.normals[3*idx.normal_index]);
                drawData.push_back(attrib.normals[3*idx.normal_index + 1]);
                drawData.push_back(attrib.normals[3*idx.normal_index + 2]);
                // Add uv
                drawData.push_back(attrib.texcoords[2*idx.texcoord_index]);
                drawData.push_back(attrib.texcoords[2*idx.texcoord_index + 1]);

                // Add collision position data
                collisionData.push_back(glm::vec3(attrib.vertices[3*idx.vertex_index], attrib.vertices[3*idx.vertex_index + 1], attrib.vertices[3*idx.vertex_index + 2]));
            }

            index_offset += fv;
        }
    }

    m_shapes.insert({shapeName, std::make_shared<Shape>(std::make_shared<VAO>(std::make_shared<VBO>(drawData), VAOAttrib::POS | VAOAttrib::NORM | VAOAttrib::UV))});

    return collisionData;
}

std::pair<std::vector<glm::vec3>, std::vector<glm::ivec3>> Graphics::getNavmeshData(std::string filepath){
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    int numTriangles = 0;
    for(size_t s = 0; s < shapes.size(); s++){
        numTriangles += shapes[s].mesh.num_face_vertices.size();
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::ivec3> faces;

    for(int i = 0; i<attrib.vertices.size(); i+=3){
        positions.push_back(glm::vec3(attrib.vertices[i], attrib.vertices[i+1], attrib.vertices[i+2]));
    }


    int i = 0;
    int j = 0;
    for(size_t s = 0; s < shapes.size(); s++) {
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            tinyobj::index_t idx1 = shapes[s].mesh.indices[3* f];
            tinyobj::index_t idx2 = shapes[s].mesh.indices[3* f + 1];
            tinyobj::index_t idx3 = shapes[s].mesh.indices[3* f + 2];
            faces.push_back(glm::ivec3(idx1.vertex_index, idx2.vertex_index, idx3.vertex_index));
        }
    }

    return std::make_pair(positions, faces);
}

void Graphics::removeShape(std::string shapeName){
    m_shapes.erase(shapeName);
}

std::shared_ptr<Shape> Graphics::getShape(std::string shapeName){
    return m_shapes.at(shapeName);
}

void Graphics::drawShape(std::shared_ptr<Shape> myShape, std::shared_ptr<ModelTransform> modelTransform, std::shared_ptr<Material> material){
    if(material == nullptr){
        m_active_shader->setMaterial(getMaterial("default"));
    }
    else{
        m_active_shader->setMaterial(material);
    }
    m_active_shader->setModelTransform(modelTransform);
    myShape->draw();
}

void Graphics::drawShape(std::shared_ptr<Shape> myShape, glm::mat4 modelMatrix, std::shared_ptr<Material> material){
    if(material == nullptr){
        m_active_shader->setMaterial(getMaterial("default"));
    }
    else{
        m_active_shader->setMaterial(material);
    }
    m_active_shader->setModelTransform(modelMatrix);
    myShape->draw();
}

std::vector<float> Graphics::getObjData(std::string filepath){
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    std::vector<glm::vec3> faces;
    std::vector<glm::vec3> vertices;

    for(size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            unsigned int fv = shapes[s].mesh.num_face_vertices[f];

            glm::vec3 face;
            for(size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                face[v] = idx.vertex_index;

            }
            faces.push_back(face);

            index_offset += fv;
        }
    }
    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        vertices.emplace_back(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]);
    }

    std::vector<float> data;
    data.resize(18 * faces.size());
    for(int i = 0; i<faces.size(); i++){
        glm::vec3 v0 = vertices[static_cast<int>(faces[i].x)];
        glm::vec3 v1 = vertices[static_cast<int>(faces[i].y)];
        glm::vec3 v2 = vertices[static_cast<int>(faces[i].z)];
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        data[18*i] = v0.x;
        data[18*i+1] = v0.y;
        data[18*i+2] = v0.z;
        data[18*i+3] = normal.x;
        data[18*i+4] = normal.y;
        data[18*i+5] = normal.z;
        data[18*i+6] = v1.x;
        data[18*i+7] = v1.y;
        data[18*i+8] = v1.z;
        data[18*i+9] = normal.x;
        data[18*i+10] = normal.y;
        data[18*i+11] = normal.z;
        data[18*i+12] = v2.x;
        data[18*i+13] = v2.y;
        data[18*i+14] = v2.z;
        data[18*i+15] = normal.x;
        data[18*i+16] = normal.y;
        data[18*i+17] = normal.z;
    }

    return data;
}

std::shared_ptr<Material> Graphics::addMaterial(std::string materialName, glm::vec3 color, float shininess){
    std::shared_ptr<Material> newMaterial = std::make_shared<Material>(color, shininess);
    m_materials.insert({materialName, newMaterial});
    return m_materials.at(materialName);
}

std::shared_ptr<Material> Graphics::addMaterial(std::string materialName, float shininess){
    std::shared_ptr<Material> newMaterial = std::make_shared<Material>(shininess);
    m_materials.insert({materialName, newMaterial});
    return m_materials.at(materialName);
}

std::shared_ptr<Material> Graphics::addMaterial(std::string materialName, std::string filePath, float shininess){
    std::shared_ptr<Texture> newTexture = std::make_shared<Texture>(filePath);
    std::shared_ptr<Material> newMaterial = std::make_shared<Material>(newTexture, shininess);
    m_materials.insert({materialName, newMaterial});
    return m_materials.at(materialName);
}

void Graphics::removeMaterial(std::string materialName){
    if(materialName != "default"){
        m_materials.erase(materialName);
    }
}

std::shared_ptr<Material> Graphics::getMaterial(std::string materialName){
    return m_materials.at(materialName);
}

std::shared_ptr<Font> Graphics::addFont(std::string fontName, std::string filepath){
    std::shared_ptr<Font> newFont = std::make_shared<Font>(filepath);
    m_fonts.insert({fontName, newFont});
    return m_fonts.at(fontName);
}

void Graphics::removeFont(std::string fontName){
    m_fonts.erase(fontName);
}

std::shared_ptr<Font> Graphics::getFont(std::string fontName){
    return m_fonts.at(fontName);
}

void Graphics::drawUIText(std::shared_ptr<Font> font, std::string text, glm::vec2 anchorPosition, AnchorPoint anchorPoint, float textBoxWidth, float fontSize, float lineSpacing, glm::vec3 textColor){
    m_active_shader->setTextUniforms(m_windowSize.x, m_windowSize.y, textColor);
    m_textRenderer->renderUIText(font, text, anchorPosition, anchorPoint, textBoxWidth, fontSize, lineSpacing, textColor);
}

void Graphics::setGlobalData(glm::vec3 globalCoeffs){
    m_active_shader->setGlobalCoeffs(globalCoeffs);
}

void Graphics::setLights(std::vector<std::shared_ptr<Light>> lights){
    m_active_shader->setLights(lights);
}

void Graphics::clearLights(){
    m_active_shader->clearLights();
}

void Graphics::setWindowSize(glm::ivec2 windowSize){
    m_windowSize = windowSize;
}

glm::ivec2 Graphics::getWindowSize(){
    return m_windowSize;
}

void Graphics::setFramebufferSize(glm::ivec2 framebufferSize){
    m_framebufferSize = framebufferSize;
    glViewport(0, 0, m_framebufferSize.x, m_framebufferSize.y);
}

glm::ivec2 Graphics::getFramebufferSize(){
    return m_framebufferSize;
}

}
