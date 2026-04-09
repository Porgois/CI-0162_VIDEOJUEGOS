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
        
        int window_width = 800;
        int window_height = 600;

        // For delta-time calculations
        int millisecs_previous_frame = 0;

        bool is_running = false;

        void setup();
        void update();
        void render();
        void runScene();
        void processInput();
  
        Game();
        ~Game();

    public:
        SDL_Renderer* renderer = nullptr;
        std::unique_ptr<AssetManager> asset_manager;
        std::unique_ptr<EventManager> event_manager;
        std::unique_ptr<ControllerManager> controller_manager;
        std::unique_ptr<SceneManager> scene_manager;
        std::unique_ptr<Registry> registry;
        sol::state lua;
        
        static Game& getInstance();
        void init();
        void run();
        void destroy();        
};


#endif // GAME_HPP