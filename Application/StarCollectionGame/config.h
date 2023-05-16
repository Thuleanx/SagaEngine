#pragma once

#include "Engine/Utils/colors/colorPalette.h"
#include <string>
#include <glm/glm.hpp>

namespace Star::FMODSettings {
const std::string bankName = "FMOD/Saga/Build/Desktop/Master.bank";
const std::string stringBankName = "FMOD/Saga/Build/Desktop/Master.strings.bank";

const std::string music = "event:/Music";
const std::string starCollect = "event:/Star Collect";

};

namespace Star {

const std::string paletteFilename = "Resources/Palettes/hollow-1x.png";
extern Saga::Palette palette;
const int fogColorIndex = 0;
const int playerColorIndex = 2;
const int starColorIndex = 3;
const int terrainColorIndex = 1;
const int lightColorIndex = 3;
const int shadowColorIndex = 0;

}
