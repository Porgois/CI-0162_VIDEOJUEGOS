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
#include "../sceneManager/sceneLoader.hpp"

const int FRAMES_PER_SECOND = 60;
const int MILLISECS_PER_FRAME = 1000 / FRAMES_PER_SECOND;

class Game {
    private:
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        static SDL_Event event;
        
        int window_width = 800;
        int window_height = 600;

        // For delta-time calculations
        int millisecs_previous_frame = 0;

        bool is_running = false;

        void setup();
        void processInput();
        void update();
        void render();
        Game();
        ~Game();

        std::unique_ptr<AssetManager> asset_manager;
        std::unique_ptr<EventManager> event_manager;
        std::unique_ptr<Registry> registry;
        sol::state lua;

        std::unique_ptr<SceneLoader> scene_loader;

    public:
        static Game& getInstance();
        std::unique_ptr<ControllerManager> controller_manager;
        void init();
        void run();
        void destroy();
};


#endif // GAME_HPP