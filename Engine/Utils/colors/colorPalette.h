#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace Saga {

class Palette {
public:
    Palette();
    Palette(std::string filename);

    glm::vec3 getColor(int index) const;
    void drawGizmos();
private:
    std::vector<glm::vec3> palette;
};

}
