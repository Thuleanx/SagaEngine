#include "systemManager.h"

namespace Saga {

SystemManager::SystemManager() {}
SystemManager::~SystemManager() {}

void SystemManager::removeStagedSystem(EventMap::Id id, Stage stage) {
	stagedSystemsMap.removeListener(stage, id); }

EventMap::Id SystemManager::addKeyboardEventSystem(int key, System<int> system) {
	return keyboardInputMap.addListener(key, std::make_shared<System<int>>(system)); }
void SystemManager::removeKeyboardEventSystem(int key, EventMap::Id id) {
	keyboardInputMap.removeListener(key, id); }

EventMap::Id SystemManager::addMouseEventSystem(int key, System<int> system) { 
	return mouseInputMap.addListener(key, std::make_shared<System<int>>(system)); }
void SystemManager::removeMouseEventSystem(int key, EventMap::Id id) {
	mouseInputMap.removeListener(key, id); }

EventMap::Id SystemManager::addMousePosSystem(System<double, double> system) {
    return otherInputMap.addListener(OtherInput::MOUSE_POS, std::make_shared<System<double,double>>(system)); }
void SystemManager::removeMousePosSystem(EventMap::Id id) {
	otherInputMap.removeListener(OtherInput::MOUSE_POS, id); }

EventMap::Id SystemManager::addScrollSystem(System<double> system) {
    return otherInputMap.addListener(OtherInput::SCROLL, std::make_shared<System<double>>(system)); }
void SystemManager::removeScrollSystem(EventMap::Id id) {
	otherInputMap.removeListener(OtherInput::SCROLL, id); }

EventMap::Id SystemManager::addWindowResizeSystem(System<int, int> system) {
    return otherInputMap.addListener(OtherInput::WINDOW_RESIZE, std::make_shared<System<int,int>>(system)); }
void SystemManager::removeWindowResizeSystem(EventMap::Id id) {
	otherInputMap.removeListener(OtherInput::WINDOW_RESIZE, id); }

}
