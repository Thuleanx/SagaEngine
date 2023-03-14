#include "invokableSystemManager.h"
#include <iostream>

namespace Saga {

void InvokableSystemManager::runStageStartup(std::shared_ptr<GameWorld> gameWorld) {
	stagedSystemsMap.invoke(Stage::Awake, gameWorld);
	stagedSystemsMap.invoke(Stage::Start, gameWorld); 
}

void InvokableSystemManager::runStageUpdate(std::shared_ptr<GameWorld> gameWorld, float time, float deltaTime) {
	stagedSystemsMap.invoke(Stage::PreUpdate, gameWorld, time, deltaTime);
	stagedSystemsMap.invoke(Stage::Update, gameWorld, time, deltaTime);
	stagedSystemsMap.invoke(Stage::LateUpdate, gameWorld, time, deltaTime); }

void InvokableSystemManager::runStageFixedUpdate(std::shared_ptr<GameWorld> gameWorld, float time, float deltaTime) {
	stagedSystemsMap.invoke(Stage::FixedUpdate, gameWorld, time, deltaTime);
	stagedSystemsMap.invoke(Stage::LateFixedUpdate, gameWorld, time, deltaTime); }

void InvokableSystemManager::runStageDraw(std::shared_ptr<GameWorld> gameWorld) {
	stagedSystemsMap.invoke(Stage::Draw, gameWorld); }

void InvokableSystemManager::keyEvent(std::shared_ptr<GameWorld> gameWorld, int key, int action) {
	keyboardInputMap.invoke(key, gameWorld, action); }

void InvokableSystemManager::mousePosEvent(std::shared_ptr<GameWorld> gameWorld, double xpos, double ypos) {
	otherInputMap.invoke(OtherInput::MOUSE_POS, gameWorld, xpos, ypos); }

void InvokableSystemManager::mouseButtonEvent(std::shared_ptr<GameWorld> gameWorld, int button, int action) {
    mouseInputMap.invoke(button, gameWorld, action); }

void InvokableSystemManager::scrollEvent(std::shared_ptr<GameWorld> gameWorld, double distance) {
	otherInputMap.invoke(OtherInput::SCROLL, gameWorld, distance); }

void InvokableSystemManager::windowResizeEvent(std::shared_ptr<GameWorld> gameWorld, int width, int height) {
	otherInputMap.invoke(OtherInput::WINDOW_RESIZE, gameWorld, width, height); }

}
