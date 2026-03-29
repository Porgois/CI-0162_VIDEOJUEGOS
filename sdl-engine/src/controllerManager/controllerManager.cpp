#include "controllerManager.hpp"

ControllerManager::ControllerManager() {
    std::cout << "[CONTROLLERMANAGER] executes constructor!" << std::endl;
}

ControllerManager::~ControllerManager() {
    std::cout << "[CONTROLLERMANAGER] executes destructor!" << std::endl;
}

void ControllerManager::clear() {
    action_key_name.clear();
    key_down.clear();
}

void ControllerManager::addActionKey(const std::string& action_name, int key_code) {
    action_key_name.emplace(action_name, key_code);
    key_down.emplace(key_code, false);
}

void ControllerManager::keyDown(int key_code) {
    auto it = key_down.find(key_code);

    if (it != key_down.end()) { // found the key
        key_down[key_code] = true; // set as down
    }
}

void ControllerManager::keyUp(int key_code) {
    auto it = key_down.find(key_code);

    if (it != key_down.end()) { // found the key
        key_down[key_code] = false; // set as up
    }
}

bool ControllerManager::isActionActive(const std::string& action) {
    auto it = action_key_name.find(action);

    if (it != action_key_name.end()) {
        int key_code = action_key_name[action];
        return key_down[key_code];
    }
    return false;
}