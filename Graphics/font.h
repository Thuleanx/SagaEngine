#pragma once

// Credit to https://learnopengl.com/In-Practice/Text-Rendering

#include "freetype/freetype.h"
#include "ft2build.h"
#include "glm/glm.hpp"
#include <GL/glew.h>
#include <map>
#include <string>

namespace GraphicsEngine {

struct Character {
    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

class Font {
public:
    Font(const std::string filepath);
    ~Font();

    Character &getCharacter(char c);

private:
    void createGlyphTexture(unsigned char c, FT_Face &face);
    std::map<char, Character> m_characters;
};

} // namespace GraphicsEngine
