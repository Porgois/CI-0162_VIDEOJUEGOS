#include "game.hpp"

// Events
#include "../events/clickEvent.hpp"

// Systems
#include "../systems/textRenderSystem.hpp"
#include "../systems/scriptSystem.hpp"
#include "../systems/flipSystem.hpp"
#include "../systems/cursorSystem.hpp"
#include "../systems/animationSystem.hpp"
#include "../systems/flashlightRenderSystem.hpp"
#include "../systems/circleCollisionSystem.hpp"
#include "../systems/boxCollisionSystem.hpp"
#include "../systems/renderBoxColliderSystem.hpp"
#include "../systems/movementSystem.hpp"
#include "../systems/childOfSystem.hpp"
#include "../systems/overlapSystem.hpp"
#include "../systems/physicsSystem.hpp"
#include "../systems/renderSystem.hpp"
#include "../systems/tileRenderSystem.hpp"
#include "../systems/mouseFollowSystem.hpp"
#include "../systems/uISystem.hpp"
#include "../systems/cameraMovementSystem.hpp"

// Constructor
Game::Game() {
    std::cout << "[GAME] Executes constructor!" << std::endl;

    registry = std::make_unique<Registry>();
    asset_manager = std::make_unique<AssetManager>();
    audio_manager = std::make_unique<AudioManager>();
    event_manager = std::make_unique<EventManager>();
    controller_manager = std::make_unique<ControllerManager>();
    scene_manager = std::make_unique<SceneManager>();
}

// Destructor
Game::~Game() {
    asset_manager.reset();
    audio_manager.reset();
    controller_manager.reset();
    event_manager.reset();
    registry.reset();
    scene_manager.reset();

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

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "[GAME] Could not initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return;
    }
    Mix_AllocateChannels(32);

    // Texture filtering
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

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

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Camera setup
    camera.x = 0;
    camera.y = 0;
    camera.w = window_width;
    camera.h = window_height;

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

    registry->addSystem<PhysicsSystem>();
    registry->addSystem<MovementSystem>();
    registry->addSystem<ChildOfSystem>();
    registry->addSystem<BoxCollisionSystem>();
    registry->addSystem<CircleCollisionSystem>();
    registry->addSystem<RenderBoxColliderSystem>();
    registry->addSystem<OverlapSystem>();

    registry->addSystem<TileRenderSystem>();
    registry->addSystem<RenderSystem>();
    registry->addSystem<FlashlightRenderSystem>(renderer, window_width, window_height);

    registry->addSystem<CursorSystem>();
    registry->addSystem<AnimationSystem>();
    registry->addSystem<CameraMovementSystem>();
    registry->addSystem<MouseFollowSystem>();

    registry->addSystem<FlipSystem>();
    registry->addSystem<TextRenderSystem>();
    registry->addSystem<UISystem>();
   
    lua.open_libraries(
        sol::lib::base,
        sol::lib::math,
        sol::lib::table,
        sol::lib::string,
        sol::lib::io
    ); // libraries

    // Game state values for script comunication
    lua["GameState"] = lua.create_table();
    lua["GameState"]["zones"] = lua.create_table();
    lua["GameState"]["dropped_bullet"] = sol::nil;
    lua["GameState"]["drop_state"] = "idle"; // states: "idle", "pending", "accepted", "rejected"
    lua["GameState"]["last_scene_name"] = std::string("");
    lua["GameState"]["zone_config_queue"] = lua.create_table();
    lua["GameState"]["slotted_bullets"] = lua.create_table();
    lua["GameState"]["revolver_cylinder"] = lua.create_table();
    for (int slot = 1; slot <= 6; ++slot) {
        lua["GameState"]["revolver_cylinder"][slot] = "empty";
    }

    // Load starting values from Lua config so they can be changed without recompiling
    sol::load_result config_result = lua.load_file("./assets/scripts/game_state_config.lua");
    if (config_result.valid()) {
        lua.script_file("./assets/scripts/game_state_config.lua");
        sol::optional<sol::table> config = lua["GameConfig"];
        if (config) {
            lua["GameState"]["player_health"] = config->get_or("player_health", 3);
            lua["GameState"]["player_max_health"] = config->get_or("player_max_health", 3);
            lua["GameState"]["player_revolver_ammo"] = config->get_or("player_revolver_ammo", 6);
            lua["GameState"]["player_max_revolver_ammo"] = config->get_or("player_max_revolver_ammo", 12);
            lua["GameState"]["player_shotgun_ammo"] = config->get_or("player_shotgun_ammo", 3);
            lua["GameState"]["player_max_shotgun_ammo"] = config->get_or("player_max_shotgun_ammo", 6);
            lua["GameState"]["weapons"] = config->get_or("weapons", std::vector<std::string>{"revolver", "shotgun"});
            lua["GameState"]["current_weapon_name"] = config->get_or("current_weapon_name", std::string("revolver"));
            lua["GameState"]["current_weapon_slot"] = config->get_or("current_weapon_slot", 1);
        }
    } else {
        std::cout << "[GAME] Failed to load game_state_config.lua, using defaults." << std::endl;
        lua["GameState"]["player_health"] = 3;
        lua["GameState"]["player_max_health"] = 3;
        lua["GameState"]["player_revolver_ammo"] = 6;
        lua["GameState"]["player_max_revolver_ammo"] = 12;
        lua["GameState"]["player_shotgun_ammo"] = 3;
        lua["GameState"]["player_max_shotgun_ammo"] = 6;
        lua["GameState"]["weapons"] = std::vector<std::string>{"revolver", "shotgun"};
        lua["GameState"]["current_weapon_name"] = std::string("revolver");
        lua["GameState"]["current_weapon_slot"] = 1;
    }

    registry->getSystem<ScriptSystem>().createLuaBindings(lua, registry, named_entities); // bindings
    scene_manager->loadScriptScenes("./assets/scripts/scenes/scenes.lua", lua); // scenes
}

static void callGameStateHook(sol::state& lua, const char* hook_name) {
    sol::optional<sol::table> game_state = lua["GameState"];
    if (!game_state) {
        return;
    }

    sol::optional<sol::function> hook = game_state.value()[hook_name];
    if (!hook) {
        return;
    }

    sol::protected_function_result result = hook.value()();
    if (!result.valid()) {
        sol::error err = result;
        std::cerr << "[GAME] Failed to call GameState." << hook_name << "(): " << err.what() << std::endl;
    }
}

void Game::loadRevolverState() {
    sol::optional<sol::table> game_state = lua["GameState"];
    if (game_state) {
        if (scene_manager) {
            game_state.value()["current_scene_name"] = scene_manager->getCurrentScene();
        } else {
            game_state.value()["current_scene_name"] = "default";
        }
    }

    callGameStateHook(lua, "restore_scene_start_state");
    callGameStateHook(lua, "load_revolver_state");
    callGameStateHook(lua, "load_player_state");
    callGameStateHook(lua, "load_player_inventory_state");
    callGameStateHook(lua, "load_revolver_player_state");
    callGameStateHook(lua, "load_shotgun_state");
    callGameStateHook(lua, "load_shotgun_player_state");
    callGameStateHook(lua, "snapshot_scene_state");
}

void Game::saveRevolverState() {
    callGameStateHook(lua, "save_player_inventory_state");
    callGameStateHook(lua, "save_player_state");
    callGameStateHook(lua, "save_revolver_state");
    callGameStateHook(lua, "save_revolver_player_state");
    callGameStateHook(lua, "save_shotgun_state");
    callGameStateHook(lua, "save_shotgun_player_state");
}

void Game::requestSceneTransition() {
    if (scene_transition_state == SceneTransitionState::None) {
        scene_transition_state = SceneTransitionState::FadeOut;
        scene_transition_alpha = 0.0f;
    }
}

void Game::updateTransition() {
    if (scene_transition_state == SceneTransitionState::None) {
        return;
    }

    float step = static_cast<float>(delta_time) / scene_enter_transition_duration;
    if (scene_transition_state == SceneTransitionState::FadeOut) {
        scene_transition_alpha += step;
        if (scene_transition_alpha >= 1.0f) {
            scene_transition_alpha = 1.0f;
            scene_transition_state = SceneTransitionState::FadeHoldBefore;
            scene_transition_hold_timer = 0.0f;
        }
    } else if (scene_transition_state == SceneTransitionState::FadeHoldBefore) {
        scene_transition_hold_timer += static_cast<float>(delta_time);
        if (scene_transition_hold_timer >= scene_transition_hold_duration_before) {
            scene_manager->stopScene();
            scene_transition_state = SceneTransitionState::FadeHoldAfter;
            scene_transition_hold_timer = 0.0f;
        }
    } else if (scene_transition_state == SceneTransitionState::FadeHoldAfter) {
        scene_transition_hold_timer += static_cast<float>(delta_time);
        if (scene_transition_hold_timer >= scene_transition_hold_duration_after) {
            scene_transition_state = SceneTransitionState::FadeIn;
        }
    } else if (scene_transition_state == SceneTransitionState::FadeIn) {
        scene_transition_alpha -= step;
        if (scene_transition_alpha <= 0.0f) {
            scene_transition_alpha = 0.0f;
            scene_transition_state = SceneTransitionState::None;
        }
    }
}

void Game::renderTransition() {
    if (scene_transition_state == SceneTransitionState::None) {
        return;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(scene_transition_alpha * 255.0f));
    SDL_Rect overlay = {0, 0, window_width, window_height};
    SDL_RenderFillRect(renderer, &overlay);
}

// Processes all kinds of input
void Game::processInput() {
    // Block input during scene transitions
    if (scene_transition_state != SceneTransitionState::None) {
        return;
    }

    SDL_Event sdl_event;
    controller_manager->updateInputStates();

    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
            case SDL_QUIT: // "X" window button press
                is_running = false;
                scene_manager->stopScene();
                break;

            case SDL_KEYDOWN:

                if (sdl_event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                    scene_manager->stopScene(); // TODO: Consider changing this for a main menu scene
                    break;
                }

                if (sdl_event.key.keysym.sym == SDLK_BACKQUOTE) {
                    is_debug_mode = !is_debug_mode;
                    break;
                }

                controller_manager->keyDown(sdl_event.key.keysym.sym);
                break;
                
            case SDL_KEYUP:
                controller_manager->keyUp(sdl_event.key.keysym.sym);
                break;

            case SDL_MOUSEMOTION:
                int x, y;
                SDL_GetMouseState(&x, &y);
                controller_manager->setMousePosition(x, y); // store in controller manager
                break;
            
            case SDL_MOUSEBUTTONDOWN:
                controller_manager->setMousePosition(sdl_event.button.x, sdl_event.button.y);
                controller_manager->mouseButtonDown(static_cast<int>(sdl_event.button.button));
                event_manager->emitEvent<ClickEvent>(
                    static_cast<int>(sdl_event.button.button),
                    sdl_event.button.x, 
                    sdl_event.button.y
                );
                break;
            
            case SDL_MOUSEBUTTONUP:
                controller_manager->setMousePosition(sdl_event.button.x, sdl_event.button.y);
                controller_manager->mouseButtonUp(static_cast<int>(sdl_event.button.button));
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
    delta_time = (SDL_GetTicks() - millisecs_previous_frame) / 1000.0;

    millisecs_previous_frame = SDL_GetTicks();
    event_manager->reset();
    registry->getSystem<OverlapSystem>().subscribeCollisionEvent(event_manager);
    registry->getSystem<UISystem>().subscribeClickEvent(event_manager, lua);
    registry->update();
    registry->getSystem<ScriptSystem>().update(lua);
    registry->update();
    registry->getSystem<ScriptSystem>().start(lua);
    
    registry->getSystem<FlipSystem>().update(camera, zoom_level);
    registry->getSystem<AnimationSystem>().update();
    registry->getSystem<PhysicsSystem>().update();
    registry->getSystem<MovementSystem>().update(delta_time);
    registry->getSystem<ChildOfSystem>().update();
    registry->getSystem<CameraMovementSystem>().update(camera, zoom_level, delta_time);
    registry->getSystem<MouseFollowSystem>().update(camera, zoom_level);
    registry->getSystem<BoxCollisionSystem>().update(event_manager, lua);
    registry->getSystem<CircleCollisionSystem>().update(event_manager);

    updateTransition();
}

// Renders the screen contents
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);

    auto tile_entities = registry->getSystem<TileRenderSystem>().getEntities();

    // Darkness
    registry->getSystem<FlashlightRenderSystem>().buildDarkness(renderer, asset_manager, camera, zoom_level);

    //Full pass (lit and unlit sprites)
    registry->getSystem<RenderSystem>().update(renderer, asset_manager, camera, zoom_level, \
        tile_entities, false, \
        registry->getSystem<FlashlightRenderSystem>().getDarknessTexture(), \
        registry->getSystem<FlashlightRenderSystem>().getScratchTexture());

    // Composite darkness
    registry->getSystem<FlashlightRenderSystem>().compositeDarkness(renderer);

    // Unlit pass
    registry->getSystem<RenderSystem>().update(renderer, asset_manager, camera, zoom_level, \
        tile_entities, true);

    registry->getSystem<TextRenderSystem>().update(renderer, asset_manager, camera, zoom_level);
    registry->getSystem<CursorSystem>().update(renderer, asset_manager);

    if (is_debug_mode) {
        registry->getSystem<RenderBoxColliderSystem>().update(renderer, camera, zoom_level);
        registry->getSystem<MouseFollowSystem>().debugRender(renderer, camera, zoom_level);
        registry->getSystem<UISystem>().debug_draw(renderer);
    }

    renderTransition();
    SDL_RenderPresent(renderer);
}

void Game::runScene() {
    scene_manager->loadScene();
    std::cout << "[SCENE] loadScene done" << std::endl;

    sol::optional<sol::table> game_state = lua["GameState"];
    if (game_state) {
        game_state.value()["current_scene_name"] = scene_manager->getCurrentScene();
    }

    registry->update();
    std::cout << "[SCENE] first registry->update() done" << std::endl;
    registry->getSystem<ScriptSystem>().start(lua);
    std::cout << "[SCENE] ScriptSystem::start() done" << std::endl;
    loadRevolverState();
    scene_manager->startScene();

    while(scene_manager->isRunning()) {
        processInput();
        update();
        render();
    }

    saveRevolverState();
    asset_manager->clearAssets();
    registry->clearAllEntities();
    named_entities.clear();
}

// Runs the actual game in a loop
void Game::run() {
    setup();

    while(is_running) {
        runScene();
    }
}

void Game::destroy() {
    if (audio_manager) {
        audio_manager->clearAssets();
    }
    Mix_CloseAudio();
    Mix_Quit();

    // Destroy Renderer and Window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Destroy TTF and SDL
    TTF_Quit();
    SDL_Quit();
}