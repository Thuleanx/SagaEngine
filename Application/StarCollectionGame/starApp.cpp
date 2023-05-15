#include "starApp.h"

namespace Star {

StarApp::StarApp() { 
    worldSetup();
}

StarApp::~StarApp() { 
}

void StarApp::worldSetup() {
    mainWorld = createGameWorld();
}

}
