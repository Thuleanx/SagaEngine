#pragma once

#include "Engine/Components/camera.h"
#include <memory>

namespace Saga {
    class GameWorld;
}

namespace Saga::Systems::Graphics {
    struct PostProcessingSettings {
        bool enabled = false;

        std::string screenFramebuffer = "screenFramebuffer";
        std::string bloomExtractionFramebuffer = "bloomExtraction";
        std::string bloomExtractionShader = "bloomExtraction";
        std::string blurShader = "gaussianBlur";
        std::string postProcessingColors = "postProcessingColors";

        int dofBlurIterationsFront = 1;
        int dofBlurIterationsBack = 1;
        int dofNearCocExpand = 20;
        int dofBlurNearCocIterations = 1;

        float focusDistance = 5;
        float focusRange = 5;

        glm::vec3 fogColor = glm::vec3(2,4,18)/255.f;
        float fogDensity = 0.03;

        float bloomIntensity = 1;
        int bloomBlurIterations = 5;
        float bloomRadius = 3;
        float bloomThreshold = 0.7;

        float exposure = 1;
        float temperature = 0;
        float tint = 0;
        float contrast = 1;
        float brightness = 0;
        float saturation = 1;

        // for tonemapping
        float gamma = 1.8;
    };

    void drawPostProcessingGizmos(std::shared_ptr<Saga::GameWorld> world);
    void postProcessingSetup(std::shared_ptr<Saga::GameWorld> world, Saga::Camera& camera);
    void usePostProcessingFBO(std::shared_ptr<GameWorld> world);
    void performPostProcessing(std::shared_ptr<Saga::GameWorld> world, Saga::Camera& camera);
}
