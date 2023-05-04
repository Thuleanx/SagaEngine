#pragma once

#include "Engine/Components/camera.h"
#include <memory>

namespace Saga {
    class GameWorld;
}

namespace Saga::Systems::Graphics {
    struct PostProcessingSettings {
        std::string screenFramebuffer = "screenFramebuffer";
        std::string bloomExtractionFramebuffer = "bloomExtraction";
        std::string bloomExtractionShader = "bloomExtraction";
        std::string blurShader = "gaussianBlur";
        std::string postProcessingColors = "postProcessingColors";

        int bloomBlurIterations = 5;
        float bloomRadius = 3;
        float bloomThreshold = 0.7;

        float exposure = 0.3;
        float temperature = 0;
        float tint = 0;
        float contrast = 1;
        float brightness = 0;
        float saturation = 1;

        // for tonemapping
        float gamma = 2.2;
    };

    void drawPostProcessingGizmos(std::shared_ptr<Saga::GameWorld> world);
    void postProcessingSetup(std::shared_ptr<Saga::GameWorld> world, Saga::Camera& camera);
    void usePostProcessingFBO(std::shared_ptr<GameWorld> world);
    void performPostProcessing(std::shared_ptr<Saga::GameWorld> world, Saga::Camera& camera);
}
