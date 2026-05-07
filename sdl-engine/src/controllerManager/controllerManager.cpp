#include "controllerManager.hpp"

ControllerManager::ControllerManager() {
    std::cout << "[CONTROLLER MANAGER] executes constructor!" << std::endl;
}

ControllerManager::~ControllerManager() {
    std::cout << "[CONTROLLER MANAGER] executes destructor!" << std::endl;
}

void ControllerManager::clear() {
    action_key_name.clear();
    key_down.clear();
    mouse_button_was_down.clear();
}

//* Keyboard
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

//* Mouse
void ControllerManager::addMouseButton(const std::string& name, int button_code) {
    mouse_button_name.emplace(name, button_code);
    mouse_button_down.emplace(button_code, false);
    mouse_button_was_down.emplace(button_code, false);
}

void ControllerManager::mouseButtonDown(int button_code) {
    auto it = mouse_button_down.find(button_code);

    if (it != mouse_button_down.end()) {
        mouse_button_down[button_code] = true;
    }
}

void ControllerManager::mouseButtonUp(int button_code) {
    auto it = mouse_button_down.find(button_code);

    if (it != mouse_button_down.end()) {
        mouse_button_down[button_code] = false;
    }
}

bool ControllerManager::isMouseButtonJustPressed(const std::string& name) {
    auto name_it = mouse_button_name.find(name);
    if (name_it == mouse_button_name.end()) {
        return false;
    }

    int button_code = name_it->second;

    auto down_it = mouse_button_down.find(button_code);
    if (down_it == mouse_button_down.end()) {
        return false;
    }

    auto was_it = mouse_button_was_down.find(button_code);
    if (was_it == mouse_button_was_down.end()) {
        return false;
    }

    return down_it->second && !was_it->second;
}

bool ControllerManager::isMouseButtonDown(const std::string& name) {
    auto name_it = mouse_button_name.find(name);
    if (name_it == mouse_button_name.end()) {
        return false;
    }

    auto code_it = mouse_button_down.find(name_it->second);
    if (code_it == mouse_button_down.end()) {
        return false;
    }

    return code_it->second;
}

void ControllerManager::updateMouseButtonStates() {
    for (auto& [key, value] : mouse_button_was_down) {
        value = mouse_button_down[key]; // snapshot current -> previous
    }
}

void ControllerManager::setMousePosition(int x, int y) {
    mouse_pos_x = x;
    mouse_pos_y = y;
}

std::tuple<int, int> ControllerManager::getMousePosition() {
    return {mouse_pos_x, mouse_pos_y};
}
