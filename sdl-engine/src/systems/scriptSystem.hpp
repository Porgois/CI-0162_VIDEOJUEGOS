#ifndef SCRIPT_SYSTEM_HPP
#define SCRIPT_SYSTEM_HPP
#include <memory>
#include <sol/sol.hpp>

#include "../components/scriptComponent.hpp"
#include "../binding/luaBinding.hpp"
#include "../e.c.s./ecs.hpp"

class ScriptSystem : public System {
public:
    bool g_is_spawning = false;
    std::unordered_map<int, sol::environment> entity_environments;

    ScriptSystem() {
        requireComponent<ScriptComponent>();
    }

    void createLuaBindings(
        sol::state& lua,
        std::unique_ptr<Registry>& registry,
        std::unordered_map<std::string, Entity>& named_entities
    ) {
        // Classes
        lua.new_usertype<Entity>("entity");

        // Controls
        lua.set_function("is_action_active", isActionActive);
        lua.set_function("is_button_pressed",  isButtonPressed);
        lua.set_function("is_button_just_pressed", isButtonJustPressed);

        // Misc
        lua.set_function("play_animation", setAnimation);
        lua.set_function("toggle_camera_follow", toggleCameraFollow);
        lua.set_function("toggle_mouse_follow", toggleMouseFollow);
        lua.set_function("toggle_sprite_flip", toggleSpriteFlip);
        lua.set_function("toggle_flashlight", toggleFlashlight);
        lua.set_function("find_entity", findEntity);
        lua.set_function("has_entity", hasEntity);
        lua.set_function("remove_box_collider", removeBoxCollider);
        lua.set_function("shake_camera", shakeCamera);

        // Setters
        lua.set_function("set_velocity", setVelocity);
        lua.set_function("set_position", setPosition);
        lua.set_function("set_rotation", setRotation);
        lua.set_function("set_text", setTextText);
        lua.set_function("set_flip", [&registry](Entity entity, bool flipped) {
            auto& sprite = entity.getComponent<SpriteComponent>();
            sprite.flip_to_mouse = false; // disable mouse control
            sprite.flip = flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        });

        // Getters
        lua.set_function("get_velocity", getVelocity);
        lua.set_function("get_position", getPosition);
        lua.set_function("get_pivoted_position", getPivotedPosition);
        lua.set_function("get_mouse_world_position", getMouseWorldPosition);
        lua.set_function("get_mouse_position", getMousePosition);
        
        lua.set_function("get_previous_position", getPreviousPosition);
        lua.set_function("get_collider_size", getColliderSize);
        lua.set_function("get_collider_offset", getColliderOffset);
        lua.set_function("get_tag", getTag);
        lua.set_function("is_flipped", getFlip);

        // Generic function-calling
        lua.set_function("call_function", [this](Entity entity, const std::string& func_name, sol::variadic_args args) {
            auto it = entity_environments.find(entity.getId());
            sol::environment env;

            if (it != entity_environments.end()) {
                env = it->second;
            } else if (entity.hasComponent<ScriptComponent>()) {
                const auto& script = entity.getComponent<ScriptComponent>();
                if (script.update.valid()) {
                    env = sol::get_environment(script.update);
                } else if (script.start.valid()) {
                    env = sol::get_environment(script.start);
                } else if (script.onCollision.valid()) {
                    env = sol::get_environment(script.onCollision);
                } else if (script.onClick.valid()) {
                    env = sol::get_environment(script.onClick);
                }
                if (env.valid()) {
                    env["this"] = entity;
                    entity_environments[entity.getId()] = env;
                }
            }

            if (!env.valid()) {
                std::cout << "[SCRIPT] call_function: no environment found for entity" << std::endl;
                return;
            }

            sol::protected_function func = env[func_name];

            if (!func.valid()) {
                std::cout << "[SCRIPT] call_function: function '" << func_name << "' not found" << std::endl;
                return;
            }

            auto result = func(args);
            if (!result.valid()) {
                sol::error err = result;
                std::cout << "[SCRIPT] call_function error: " << err.what() << std::endl;
            }
        });

        // Delta time
        lua.set_function("get_delta_time", [&game = Game::getInstance()]() {
            return game.delta_time;
        });

        // Scene
        lua.set_function("go_to_scene", goToScene);

        // Entity creation & deletion
        lua.set_function("spawn_entity", [&lua, &registry, &named_entities](const sol::table& entity_def) -> Entity {
            return SceneLoader::createEntity(lua, entity_def, registry, named_entities);
        });
                
        lua.set_function("delete_entity", [&registry, &named_entities](Entity entity) {
            auto it = std::find_if(named_entities.begin(), named_entities.end(),
                [&entity](const auto& pair) {
                    return pair.second == entity;
                });

            if (it != named_entities.end()) {
                named_entities.erase(it);
            }

            registry->destroyEntity(entity);
        });
    }
    
    void start(sol::state& lua) {
        
        for (auto entity : getSystemEntities()) {
            auto& script = entity.getComponent<ScriptComponent>();
            if (script.started) continue;
            script.started = true;

            if (script.start.valid()) {
                // Get the environment the function was defined in
                sol::environment env = sol::get_environment(script.start);
                if (env.valid()) {
                    env["this"] = entity;
                    entity_environments[entity.getId()] = env;
                } else {
                    lua["this"] = entity;
                }
                auto result = script.start();
                if (!result.valid()) {
                    sol::error err = result;
                    std::cout << "[SCRIPT] start() error: " << err.what() << std::endl;
                }
            }
        }
    }

    void update(sol::state& lua) {
        for (auto entity : getSystemEntities()) {
            const auto& script = entity.getComponent<ScriptComponent>();
            if (script.update.valid()) {
                sol::environment env = sol::get_environment(script.update);
                if (env.valid()) {
                    env["this"] = entity;
                    entity_environments[entity.getId()] = env;
                } else {
                    lua["this"] = entity;
                }
                auto result = script.update();
                if (!result.valid()) {
                    sol::error err = result;
                    std::cout << "[SCRIPT] update() error: " << err.what() << std::endl;
                }
            }
        }
    }
};

#endif // SCRIPT_SYSTEM_HPP