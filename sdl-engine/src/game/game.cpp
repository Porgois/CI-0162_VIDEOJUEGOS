#include "game.hpp"

// Components
#include "../components/scriptComponent.hpp"
#include "../components/cursorComponent.hpp"
#include "../components/animationComponent.hpp"
#include "../components/spriteComponent.hpp"
#include "../components/transformComponent.hpp"
#include "../components/rigidBodyComponent.hpp"
#include "../components/flashlightComponent.hpp"
#include "../components/circleColliderComponent.hpp"
#include "../components/rectangleColliderComponent.hpp"
#include "../components/colliderComponent.hpp"

// Systems
#include "../systems/scriptSystem.hpp"
#include "../systems/flipSystem.hpp"
#include "../systems/cursorSystem.hpp"
#include "../systems/animationSystem.hpp"
#include "../systems/flashlightRenderSystem.hpp"
#include "../systems/collisionSystem.hpp"
#include "../systems/movementSystem.hpp"
#include "../systems/renderSystem.hpp"
#include "../systems/damageSystem.hpp"

// Constructor
Game::Game() {
    std::cout << "[GAME] Executes constructor!" << std::endl;

    registry = std::make_unique<Registry>();
    asset_manager = std::make_unique<AssetManager>();
    event_manager = std::make_unique<EventManager>();
    controller_manager = std::make_unique<ControllerManager>();
}

// Destructor
Game::~Game() {
    asset_manager.reset();
    controller_manager.reset();
    event_manager.reset();
    registry.reset();

    std::cout << "[GAME] Executes destructor!" << std::endl;
}

// Get instance
Game& Game::getInstance() {
    static Game game;
    return game;
}

void Game::init() {
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { // 0 -> no issues
        std::cerr << "[GAME] Could not initialize SDL2!" << std::endl;
        return;
    }

    // Initialize TTF
    if (TTF_Init() != 0) {
        std::cerr << "[GAME] Could not initialize SDL_TTF!" << std::endl;
        return;
    }

    // Create the game window
    window = SDL_CreateWindow(
        "2D Game Engine",
        SDL_WINDOWPOS_CENTERED, // X appear pos
        SDL_WINDOWPOS_CENTERED, // Y appear pos
        window_width,
        window_height,
        SDL_WINDOW_SHOWN // Flags: https://wiki.libsdl.org/SDL2/SDL_WindowFlags

    );

    if (!window) {
        std::cerr << "[GAME] Could not create the game *window*!" << std::endl;
        return;
    }

    // Create renderer (ver https://wiki.libsdl.org/SDL2/SDL_Renderer)
    renderer = SDL_CreateRenderer(
        window, // window to render to
        -1, // screen driver; [the index of the rendering driver]
        0 // flags (no flags) https://wiki.libsdl.org/SDL2/SDL_RendererFlags
    );

    if (!renderer) {
        std::cerr << "[GAME] Could not create the game *renderer*!" << std::endl;
        return;
    }

    // The game is running
    is_running = true;   
}

// initiliaze all necessary stuff
void Game::setup() {
    // Add systems
    registry->addSystem<ScriptSystem>();
    registry->addSystem<CursorSystem>();
    registry->addSystem<AnimationSystem>();
    registry->addSystem<RenderSystem>();
    registry->addSystem<MovementSystem>();
    registry->addSystem<CollisionSystem>();
    registry->addSystem<DamageSystem>();
    registry->addSystem<FlashlightRenderSystem>(renderer, window_width, window_height);
    registry->addSystem<FlipSystem>();

    // Add lua libraries
    lua.open_libraries(sol::lib::base, sol::lib::math);

    // Bind
    registry->getSystem<ScriptSystem>().createLuaBinding(lua);

    // Add textures
    asset_manager->addTexture(renderer, "crosshair", "./assets/ui/cursors/cursor.png");
    asset_manager->addTexture(renderer, "player", \
        "./assets/sprites/characters/player/player_sprite_sheet.png");
    asset_manager->addTexture(renderer, "barrel", "./assets/sprites/environment/barrel_sprite.png");
    asset_manager->addTexture(renderer, "flashlight-cone", "./assets/sprites/masks/cone.png");
    asset_manager->addTexture(renderer, "flashlight-source", "./assets/sprites/masks/circle.png");

    // Add inputs
    controller_manager->addActionKey("move_up", 119); // SDLK_w
    controller_manager->addActionKey("move_left", 97); // SDLK_a
    controller_manager->addActionKey("move_down", 115); // SDLK_s
    controller_manager->addActionKey("move_right", 100); // SDLK_d
    
    // Lua script
    lua.script_file("./assets/scripts/player.lua");
    sol::function update = lua["update"];

    // Cursor entity
    Entity cursor = registry->createEntity("cursor");
    
    cursor.addComponent<SpriteComponent>("crosshair", 4, 4, 0, 0);
    cursor.addComponent<CursorComponent>();
    
    // Player
    Entity player = registry->createEntity("player");
    std::cout << "Player name: " << player.getName() << std::endl;

    player.addComponent<ScriptComponent>(update);
    player.addComponent<TransformComponent> (glm::vec2(325.0, 215.0), glm::vec2(3.0, 3.0), 0.0);
    player.addComponent<SpriteComponent>("player", 30, 30, 0, 0, true);
    player.addComponent<AnimationComponent>();
    player.addComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    player.addComponent<FlashlightComponent>("flashlight-cone", "flashlight-source", 520, 275);
    player.addComponent<ColliderComponent>();
    player.addComponent<CircleColliderComponent>(5, 30, 30);

    // Animations
    auto& anim = player.getComponent<AnimationComponent>();
    anim.clips["idle"] = {0, 4, 8, true};
    anim.clips["walk"] = {1, 6, 13, true};

    // Barrel
    Entity barrel = registry->createEntity("barrel");
    barrel.addComponent<TransformComponent> (glm::vec2(30, 80), glm::vec2(5.0, 5.0), 0.0);
    barrel.addComponent<SpriteComponent>("barrel", 13, 16, 0, 0, false);
    barrel.addComponent<ColliderComponent>();
    
    //(sprite base formula)
    //offset.x = (sprite_width  - collider_width)  / 2   ← center horizontally
    //offset.y =  sprite_height - collider_height         ← push to bottom

    barrel.addComponent<RectangleColliderComponent>(12, 8, glm::vec2(0.5, 7.0));
}

// Processes all kinds of input
void Game::processInput() {
    SDL_Event sdl_event;

    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
            case SDL_QUIT: // "X" window button press
                is_running = false;
                break;

            case SDL_KEYDOWN:
                if (sdl_event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                    break;
                }
                controller_manager->keyDown(sdl_event.key.keysym.sym);
                break;

            case SDL_KEYUP:
                controller_manager->keyUp(sdl_event.key.keysym.sym);
                break;

            default:
                break;
        }
    }
}

// Moment to moment game update
void Game::update() {
    // calculate wait time
    int time_to_wait = MILLISECS_PER_FRAME - (SDL_GetTicks() \
        - millisecs_previous_frame);
    
    // create a delay if going faster than expected
    if (0 < time_to_wait && time_to_wait <= MILLISECS_PER_FRAME) {
        SDL_Delay(time_to_wait);
    }

    // set delta_time for use throughout
    double delta_time = (SDL_GetTicks() - millisecs_previous_frame) / 1000.0;
    //TODO: Add to LUA state

    millisecs_previous_frame = SDL_GetTicks();
    event_manager->reset();
    //registry->getSystem<DamageSystem>().subscribeCollisionEvent(event_manager);

    registry->update();
    registry->getSystem<ScriptSystem>().update(lua);
    
    registry->getSystem<FlipSystem>().update();
    registry->getSystem<AnimationSystem>().update();
    registry->getSystem<MovementSystem>().update(delta_time);
    registry->getSystem<CollisionSystem>().update(event_manager);
}

// Renders the screen contents
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 117, 167, 67, 255); // greenish background
    SDL_RenderClear(renderer);
    
    registry->getSystem<RenderSystem>().update(renderer, asset_manager);
    //registry->getSystem<FlashlightRenderSystem>().update(renderer, asset_manager);
    registry->getSystem<CursorSystem>().update(renderer, asset_manager);

    SDL_RenderPresent(renderer);
}

// Runs the actual game in a loop
void Game::run() {
    setup();

    while(is_running) {
        processInput();
        update();
        render();
    }
}

void Game::destroy() {
    // Destroy Renderer and Window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Destroy TTF and SDL
    TTF_Quit();
    SDL_Quit();
}