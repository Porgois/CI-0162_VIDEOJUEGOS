
#include <iostream>

#include <SDL2/SDL.h>
#include <sol/sol.hpp>
#include <glm/glm.hpp>

#include "game/game.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


// Handle exit-related inputs
void handle_exit_inputs(bool *is_running, SDL_Event *sdl_event) {
    switch(sdl_event->type) {
        // Handle exit
        case SDL_QUIT:
            *is_running = false;
            break;
        case SDL_KEYDOWN:
            if (sdl_event->key.keysym.sym == SDLK_ESCAPE) {
                *is_running = false;
            }
            break;
    }
}

// Handles all sorts of events
void updateEventRegistry(bool* is_running) {
    SDL_Event sdl_event;

    // Takes a pending event from the sdl event queue and stores its data on a registry of type "SDL_Event"
    while(SDL_PollEvent(&sdl_event)) {
        handle_exit_inputs(is_running, &sdl_event);
    }
}

// Creates an SDL window of width x height pixels given
void create_window() {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    bool is_running = true;


    // Create SDL square
    SDL_Rect square = {
        (WINDOW_WIDTH / 2) - 25, // x position
        (WINDOW_HEIGHT / 2) - 25, // y position
        50, // square width & height
        50
    };

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { // 0 -> no issues
        std::cerr << "Could not initialize SDL2!" << std::endl;
        SDL_Quit();
        return;
    }

    // Creating actual rendered window
    window = SDL_CreateWindow(
        "Engine",
        SDL_WINDOWPOS_CENTERED, // X appear pos
        SDL_WINDOWPOS_CENTERED, // Y appear pos
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN // Flags: https://wiki.libsdl.org/SDL2/SDL_WindowFlags

    );

    // Create renderer (ver https://wiki.libsdl.org/SDL2/SDL_Renderer)
    renderer = SDL_CreateRenderer(
        window, // window to render to
        -1, // screen driver; [the index of the rendering driver]
        0 // flags (no flags) https://wiki.libsdl.org/SDL2/SDL_RendererFlags
    );

    // ----- Run ------
    while (is_running) {
        updateEventRegistry(&is_running);

        // Draw color (https://wiki.libsdl.org/SDL3/SDL_SetRenderDrawColor)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White, no transparency

        // Clean window (https://wiki.libsdl.org/SDL2/SDL_RenderClear)
        SDL_RenderClear(renderer);


        // Draw the previously-created square on screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black
        SDL_RenderFillRect(renderer, &square);

        // Update window
        SDL_RenderPresent(renderer); // https://wiki.libsdl.org/SDL2/SDL_RenderPresent

    }

    // Memory cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;


    // Runs the game class
    Game& game = Game::getInstance();
    game.init();
    game.run();
    game.destroy();
 
    return 0;
}
