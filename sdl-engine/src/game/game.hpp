#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <sol/sol.hpp>
#include <memory>
#include <iostream>

// Managers
#include "../e.c.s./ecs.hpp"
#include "../assetManager/assetManager.hpp"
#include "../eventManager/eventManager.hpp"
#include "../controllerManager/controllerManager.hpp"
#include "../sceneManager/sceneManager.hpp"

const int FRAMES_PER_SECOND = 60;
const int MILLISECS_PER_FRAME = 1000 / FRAMES_PER_SECOND;

class Game {
    private:
        SDL_Window* window = nullptr;
        static SDL_Event event;
        
        // For delta-time calculations
        int millisecs_previous_frame = 0;

        bool is_running = false;
        bool is_debug_mode = false;

        void setup();
        void update();
        void render();
        void runScene();
        void processInput();
  
        Game();
        ~Game();

    public:
        int window_width = 800;
        int window_height = 600;
        int map_width = 0;
        int map_height = 0;
        float zoom_level = 2.5;
        double delta_time = 0.0;
        SDL_Renderer* renderer = nullptr;
        SDL_Rect camera = {0, 0, 0, 0};
        std::unique_ptr<AssetManager> asset_manager;
        std::unique_ptr<EventManager> event_manager;
        std::unique_ptr<ControllerManager> controller_manager;
        std::unique_ptr<SceneManager> scene_manager;
        std::unique_ptr<Registry> registry;
        sol::state lua;
        std::unordered_map<std::string, Entity> named_entities;
        
        static Game& getInstance();
        void init();
        void run();
        void destroy();        
};


#endif // GAME_HPP