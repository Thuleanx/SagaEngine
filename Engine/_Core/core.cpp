#include "core.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include <utility>
#include "app.h"
#include "Application/Platformer/app.h"
#include "logger.h"
#include "../Audio/audioEngine.h"

using namespace std;

namespace Saga {

Core::Core() {
	SINFO("Start initializing application.");
	AudioEngine::init();
	application = make_shared<Platformer::App>();
}

Core::~Core() {
	application = nullptr; // release application
	AudioEngine::release();
}

void Core::update(double deltaTime, double time) {
	AudioEngine::update();
	application->update(deltaTime, time);
}

void Core::fixedupdate(double fixedDeltaTime, double time) {
	application->fixedUpdate(fixedDeltaTime, time);
}

void Core::draw() {
	application->draw();
}

void Core::keyEvent(int key, int action) {
    application->keyEvent(key, action);
}

void Core::mousePosEvent(double xpos, double ypos) {
    application->mousePosEvent(xpos, ypos);
}

void Core::mouseButtonEvent(int button, int action) {
    application->mouseButtonEvent(button, action);
}

void Core::scrollEvent(double distance) {
    application->scrollEvent(distance);
}

void Core::framebufferResizeEvent(int width, int height) {
    GraphicsEngine::Global::graphics.setFramebufferSize(glm::ivec2(width, height));
	application->framebufferResizeEvent(width, height);
}

void Core::windowResizeEvent(int width, int height) {
	GraphicsEngine::Global::graphics.setWindowSize(glm::ivec2(width, height));
	application->windowResizeEvent(width, height);
}

} // namespace Saga
