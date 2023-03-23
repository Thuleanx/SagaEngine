#include "app.h"
#include <iostream>
#include "../MetaSystems/_import.h"
#include "logger.h"
#include "asserts.h"
using namespace std;

namespace Saga {
App::App() {
}

App::~App() {
	for (auto& world : worlds) 
        if (world->started) 
			world->runStageCleanup();
}

App::AppExclusiveGameWorld::~AppExclusiveGameWorld() {
	// run the cleanup systems
}

void App::update(float deltaTime, float time) {
	for (auto& world : worlds) {
        if (!world->started) {
			world->runStageStartup();
			world->started = true;
		} else 
			world->runStageUpdate(deltaTime, time);
	}
}

void App::fixedUpdate(float deltaTime, float time) {
	for (auto& world : worlds) 
        if (world->started)
			world->runStageFixedUpdate(deltaTime, time);
}

void App::draw() {
	for (auto& world : worlds) 
        if (world->started)
			world->runStageDraw();
}

void App::keyEvent(int key, int action) {
	for (auto& world : worlds) 
        if (world->started)
			world->keyEvent(key, action);
}

void App::mousePosEvent(double xpos, double ypos) {
	for (auto& world : worlds) 
        if (world->started)
			world->mousePosEvent(xpos, ypos);
}

void App::mouseButtonEvent(int button, int action) {
	for (auto& world : worlds) 
        if (world->started)
			world->mouseButtonEvent(button, action);
}

void App::scrollEvent(double distance) {
	for (auto& world : worlds) 
        if (world->started)
			world->scrollEvent(distance);
}

void App::framebufferResizeEvent(int width, int height) {
}

void App::windowResizeEvent(int width, int height) {
	for (auto& world : worlds) 
        if (world->started)
			world->windowResizeEvent(width, height);
}

std::shared_ptr<GameWorld> App::createGameWorld() {
    std::shared_ptr<GameWorld> world = worlds.emplace_back(std::make_shared<AppExclusiveGameWorld>());

	Physics::registerPhysicsMetaSystem(world);

	return world;
}

void App::removeGameWorld(std::shared_ptr<GameWorld> world) {
	worlds.erase(std::find(worlds.begin(), worlds.end(), world));
}

void App::AppExclusiveGameWorld::runStageStartup() {
	systemManager.runStageStartup(shared_from_this()); 
}

void App::AppExclusiveGameWorld::runStageUpdate(float deltaTime, float time) {
	systemManager.runStageUpdate(shared_from_this(), deltaTime, time);
}

void App::AppExclusiveGameWorld::runStageFixedUpdate(float deltaTime, float time) {
	systemManager.runStageFixedUpdate(shared_from_this(), deltaTime, time);
}

void App::AppExclusiveGameWorld::runStageDraw() {
	systemManager.runStageDraw(shared_from_this());
}

void App::AppExclusiveGameWorld::runStageCleanup() {
	systemManager.runStageCleanup(shared_from_this());
}

void App::AppExclusiveGameWorld::keyEvent(int key, int action) {
	systemManager.keyEvent(shared_from_this(), key, action);
}

void App::AppExclusiveGameWorld::mousePosEvent(double xpos, double ypos) {
	systemManager.mousePosEvent(shared_from_this(), xpos, ypos);
}

void App::AppExclusiveGameWorld::mouseButtonEvent(int button, int action) {
	systemManager.mouseButtonEvent(shared_from_this(), button, action);
}

void App::AppExclusiveGameWorld::scrollEvent(double distance) {
	systemManager.scrollEvent(shared_from_this(), distance);
}

void App::AppExclusiveGameWorld::windowResizeEvent(int width, int height) {
	systemManager.windowResizeEvent(shared_from_this(), width, height);
}


} // namespace Saga
