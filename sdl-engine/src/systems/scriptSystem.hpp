#ifndef SCRIPT_SYSTEM_HPP
#define SCRIPT_SYSTEM_HPP
#include <cstdint>
#include <memory>
#include <sol/sol.hpp>
#include <vector>

#include "../components/scriptComponent.hpp"
#include "../binding/luaBinding.hpp"
#include "../e.c.s./ecs.hpp"

class ScriptSystem : public System {
public:
    bool g_is_spawning = false;
    std::unordered_map<uint64_t, sol::environment> entity_environments;

    void setSpriteIndex(Entity entity, int index) {
        if (!entity.hasComponent<SpriteComponent>()) {
            return;
        }

        entity.getComponent<SpriteComponent>().z_index = index;
    }

    static uint64_t getEnvironmentKey(const Entity& entity) {
        return (static_cast<uint64_t>(entity.getId()) << 32) |
               static_cast<uint64_t>(entity.getGeneration());
    }

    ScriptSystem() {
        requireComponent<ScriptComponent>();
    }

    sol::optional<sol::environment> getScriptEnvironment(Entity entity) const {
        if (!entity.hasComponent<ScriptComponent>()) {
            return sol::nullopt;
        }

        const auto& script = entity.getComponent<ScriptComponent>();
        if (script.env.valid()) {
            return script.env;
        }

        uint64_t key = getEnvironmentKey(entity);
        auto it = entity_environments.find(key);
        if (it != entity_environments.end()) {
            return it->second;
        }

        if (script.update.valid()) {
            return sol::get_environment(script.update);
        }
        if (script.start.valid()) {
            return sol::get_environment(script.start);
        }
        if (script.onCollision.valid()) {
            return sol::get_environment(script.onCollision);
        }
        if (script.onClick.valid()) {
            return sol::get_environment(script.onClick);
        }

        return sol::nullopt;
    }

    void createLuaBindings(
        sol::state& lua,
        std::unique_ptr<Registry>& registry,
        std::unordered_map<std::string, Entity>& named_entities
    ) {
        //* Classes
        lua.new_usertype<Entity>("entity");

        //* Controls
        lua.set_function("is_action_active", isActionActive);
        lua.set_function("is_action_just_pressed", isActionJustPressed);
        lua.set_function("is_button_pressed",  isButtonPressed);
        lua.set_function("is_button_just_pressed", isButtonJustPressed);

        //* Misc
        lua.set_function("play_animation", setAnimation);
        lua.set_function("toggle_camera_follow", toggleCameraFollow);
        lua.set_function("toggle_mouse_follow", toggleMouseFollow);
        lua.set_function("toggle_sprite_flip", toggleSpriteFlip);
        lua.set_function("toggle_flashlight", toggleFlashlight);
        lua.set_function("set_sprite_z_index", [this](Entity entity, int index) {
            this->setSpriteIndex(entity, index);
        });
        lua.set_function("find_entity", findEntity);
        lua.set_function("has_entity", hasEntity);
        lua.set_function("remove_box_collider", removeBoxCollider);
        lua.set_function("shake_camera", shakeCamera);
        
        //* Animation
        lua.set_function("is_animation_finished", isAnimationFinished);

        //* Audio
        lua.set_function("play_audio", sol::overload(
            [](const std::string& path) { playAudio(path, 0, 128); },
            [](const std::string& path, int loops) { playAudio(path, loops, 128); },
            [](const std::string& path, int loops, int volume) { playAudio(path, loops, volume); }
        ));
        
        lua.set_function("play_music", sol::overload(
            [](const std::string& path) { playMusic(path, -1, 128); },
            [](const std::string& path, int loops) { playMusic(path, loops, 128); },
            [](const std::string& path, int loops, int volume) { playMusic(path, loops, volume); }
        ));
        
        lua.set_function("music", sol::overload(
            [](const std::string& path) { playMusic(path, -1, 128); },
            [](const std::string& path, int loops) { playMusic(path, loops, 128); },
            [](const std::string& path, int loops, int volume) { playMusic(path, loops, volume); }
        ));
        
        lua.set_function("play_random_audio", sol::overload(
            [](const sol::table& paths) { 
                std::vector<std::string> vec;
                for (auto& p : paths) {
                    vec.push_back(p.second.as<std::string>());
                }
                playRandomAudio(vec, 0, 128); 
            },
            [](const sol::table& paths, int loops) { 
                std::vector<std::string> vec;
                for (auto& p : paths) {
                    vec.push_back(p.second.as<std::string>());
                }
                playRandomAudio(vec, loops, 128); 
            },
            [](const sol::table& paths, int loops, int volume) { 
                std::vector<std::string> vec;
                for (auto& p : paths) {
                    vec.push_back(p.second.as<std::string>());
                }
                playRandomAudio(vec, loops, volume); 
            }
        ));
        
        lua.set_function("stop_music", stopMusic);
        lua.set_function("stop_all_sounds", stopAllSounds);

        //* Setters
        lua.set_function("set_velocity", setVelocity);
        lua.set_function("set_position", setPosition);
        lua.set_function("set_rotation", setRotation);
        lua.set_function("set_child_of_offset", setChildOfOffset);
        lua.set_function("set_text", setTextText);
        lua.set_function("set_flip", [&registry](Entity entity, bool flipped) {
            auto& sprite = entity.getComponent<SpriteComponent>();
            sprite.flip_to_mouse = false; // disable mouse control
            sprite.flip = flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        });

        //* Getters
        lua.set_function("get_velocity", getVelocity);
        lua.set_function("get_position", getPosition);
        lua.set_function("get_child_of_offset", getChildOfOffset);
        lua.set_function("get_pivoted_position", getPivotedPosition);
        lua.set_function("get_mouse_world_position", getMouseWorldPosition);
        lua.set_function("get_mouse_position", getMousePosition);
        
        lua.set_function("get_previous_position", getPreviousPosition);
        lua.set_function("get_collider_size", getColliderSize);
        lua.set_function("get_collider_offset", getColliderOffset);
        lua.set_function("get_tag", getTag);
        lua.set_function("is_flipped", getFlip);

        lua.set_function("get_script", [this, &lua](Entity entity) -> sol::object {
            auto env = getScriptEnvironment(entity);
            if (!env) {
                return sol::nil;
            }
            return sol::object(lua, env.value());
        });
        lua.set_function("get_script_variable", [this, &lua](Entity entity, const std::string& key) -> sol::object {
            auto env = getScriptEnvironment(entity);
            if (!env) {
                return sol::nil;
            }
            sol::object value = env.value()[key];
            return value.valid() ? value : sol::nil;
        });

        lua.set_function("set_script_variable", [this](Entity entity, const std::string& key, sol::object value) {
            auto env = getScriptEnvironment(entity);
            if (!env) {
                return false;
            }
            env.value()[key] = value;
            return true;
        });

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
                    entity_environments[getEnvironmentKey(entity)] = env;
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
        lua.set_function("go_to_scene", sol::overload(
            [](const std::string& scene_name) { goToScene(scene_name, false, -1.0f); },
            [](const std::string& scene_name, bool fade) { goToScene(scene_name, fade, -1.0f); },
            [](const std::string& scene_name, bool fade, float hold_duration) { goToScene(scene_name, fade, hold_duration); }
        ));

        // Entity creation & deletion
        lua.set_function("spawn_entity", [&lua, &registry, &named_entities](const sol::table& entity_def) -> Entity {
            Entity entity = SceneLoader::createEntity(lua, entity_def, registry, named_entities);
            sol::optional<sol::table> components = entity_def["components"];
            if (components != sol::nullopt) {
                SceneLoader::loadChildOf(entity, components.value(), named_entities);
            }
            return entity;
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
                    entity_environments[getEnvironmentKey(entity)] = env;
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
                    entity_environments[getEnvironmentKey(entity)] = env;
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