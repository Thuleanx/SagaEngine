#pragma once

#include <memory>

namespace Saga {
class GameWorld;
}

namespace Star::Systems {

void drawEditor(std::shared_ptr<Saga::GameWorld> world);

}
