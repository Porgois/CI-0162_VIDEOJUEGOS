#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include <map>
#include <memory>
#include <sol/sol.hpp>
#include <string>

#include "./sceneLoader.hpp"

class SceneManager {
    private:
        std::map<std::string, std::string> scenes;
        std::unique_ptr<SceneLoader> scene_loader;
        std::string next_scene;
        bool is_running = false;
     
    public:
       SceneManager();
       ~SceneManager();

        void loadScriptScenes(const std::string& path, sol::state& lua);
        void loadScene();

        std::string getNextScene() const;
        void setNextScene(const std::string& next_scene);

        bool isRunning() const;
        void startScene();
        void stopScene();
};

#endif // SCENE_MANAGER_HPP