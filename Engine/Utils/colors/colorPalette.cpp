#include "colorPalette.h"

#include "Engine/_Core/logger.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "stb_image.h"
#include <iostream>
#include <string>

namespace Saga {

Palette::Palette() { }

Palette::Palette(std::string filepath) {
    int width, height, numChannels;
    unsigned char *data =
        stbi_load(filepath.c_str(), &width, &height, &numChannels, STBI_rgb);
    if (stbi_failure_reason()) {
        std::cerr << stbi_failure_reason() << std::endl;
        SERROR(stbi_failure_reason());
        return;
    }
    if (data == NULL) return;
    int numColors = width * height;
    palette.reserve(numColors);

    for (int i = 0; i < width * height; i++) {
        glm::vec3 col = glm::vec3(data[i*numChannels], data[i*numChannels+1], data[i*numChannels+2]) / 255.0f;
        palette.push_back(col);
    }
    SINFO("loaded palette at %s with %d colors and %d channels", filepath.c_str(), numColors, numChannels);
}

glm::vec3 Palette::getColor(int index) const {
    if (index < 0 || index >= palette.size()) {
        SERROR("Tried to access palette at index %d when palette only has %d colors.", index, palette.size());
        return glm::vec3(0,0,0);
    }
    return palette.at(index);
}

void Palette::drawGizmos() {
    if (ImGui::CollapsingHeader("Palette")) {
        for (int i = 0; i < palette.size(); i++) {
            std::string title = "[" + std::to_string(i) + "]";
            ImGui::ColorPicker3(title.c_str(), glm::value_ptr(palette[i]));
        }
    }
}

}
