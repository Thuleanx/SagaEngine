#include "editor.h"
#include "Application/StarCollectionGame/ECS/player/player.h"
#include "Application/StarCollectionGame/config.h"
#include "Engine/Gameworld/gameworld.h"
#include "imgui.h"

namespace Star::Systems {

void drawEditor(std::shared_ptr<Saga::GameWorld> world) {
    ImGui::Begin("Star game gizmos");
    for (Star::Player& player : *world->viewAll<Star::Player>()) {
        ImGui::BeginDisabled(true);
        int numStarsCollected = player.starsCollected.size();
        ImGui::InputInt("num stars", &numStarsCollected);
        ImGui::EndDisabled();
    }
    palette.drawGizmos();
    ImGui::End();
}

}
