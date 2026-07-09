#include <iostream>
#include "./sceneManager.hpp"
#include "../game/game.hpp"

SceneManager::SceneManager() {
    std::cout << "[SCENE MANAGER] Executes constructor." << std::endl;
    scene_loader = std::make_unique<SceneLoader>();
}

SceneManager::~SceneManager() {
    std::cout << "[SCENE MANAGER] Executes destructor." << std::endl;
    scene_loader.reset();
}

void SceneManager::loadScriptScenes(const std::string& path, sol::state& lua) {
    // TODO: Verify that the code is correct
    lua.script_file(path);

    sol::table scenes = lua["scenes"];

    int index = 0;
    while (true) {
        sol::optional<sol::table> has_scene = scenes[index];

        if (has_scene == sol::nullopt) {
            break;
        }

        sol::table scene = scenes[index];
        std::string scene_name = scene["name"];
        this->scenes.emplace(scene_name, scene["path"]);
        this->scene_order.push_back(scene_name);

        if (index == 0) {
            next_scene = scene["name"];
            current_scene = scene["name"];
        }
        index++;
    }
}

void SceneManager::loadScene() {
    Game& game = Game::getInstance();
    current_scene = next_scene;
    std::string scene_path = scenes[next_scene];
    scene_loader->loadScene(scene_path, game.lua, game.asset_manager, \
        game.controller_manager, game.registry, game.renderer);


}

std::string SceneManager::getNextScene() const {
    return next_scene;
}

std::string SceneManager::getCurrentScene() const {
    return current_scene;
}

std::string SceneManager::getNextSceneInList() const {
    if (current_scene.empty()) {
        return "";
    }

    for (size_t i = 0; i < scene_order.size(); ++i) {
        if (scene_order[i] == current_scene && i + 1 < scene_order.size()) {
            return scene_order[i + 1];
        }
    }

    return "";
}

void SceneManager::setNextScene(const std::string& next_scene) {
    this->next_scene = next_scene;
}

bool SceneManager::isRunning() const {
    return is_running;
}

void SceneManager::startScene() {
    is_running = true;
}

void SceneManager::stopScene() {
    is_running = false;
}
