#include "editor.h"
#include "Application/StarCollectionGame/ECS/player/player.h"
#include "Engine/Gameworld/gameworld.h"
#include "imgui.h"

namespace Star::Systems {

void drawEditor(std::shared_ptr<Saga::GameWorld> world) {
    for (Star::Player& player : *world->viewAll<Star::Player>()) {
        ImGui::Begin("Player");
        ImGui::BeginDisabled(true);
        ImGui::InputInt("num stars", &player.numStarsCollected);
        ImGui::EndDisabled();
        ImGui::End();
    }
}

}
