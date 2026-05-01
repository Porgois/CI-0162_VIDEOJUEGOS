#ifndef SCRIPT_SYSTEM_HPP
#define SCRIPT_SYSTEM_HPP
#include <memory>
#include <sol/sol.hpp>

#include "../components/scriptComponent.hpp"
#include "../binding/luaBinding.hpp"
#include "../e.c.s./ecs.hpp"

class ScriptSystem : public System {
public:
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
        lua.set_function("is_button_active",  isButtonPressed);
        lua.set_function("is_button_just_pressed", isButtonJustPressed);

        // Misc
        lua.set_function("play_animation", setAnimation);

        // Setters
        lua.set_function("set_velocity", setVelocity);
        lua.set_function("set_position", setPosition);
        lua.set_function("set_rotation", setRotation);

        // Getters
        lua.set_function("get_velocity", getVelocity);
        lua.set_function("get_position", getPosition);
        lua.set_function("get_pivoted_position", getPivotedPosition);
        lua.set_function("get_mouse_position", getMouseWorldPosition);
        
        lua.set_function("get_previous_position", getPreviousPosition);
        lua.set_function("get_collider_size", getColliderSize);
        lua.set_function("get_collider_offset", getColliderOffset);
        lua.set_function("get_tag", getTag);
        lua.set_function("is_flipped", getFlip);

        // Delta time
        lua.set_function("get_delta_time", [&game = Game::getInstance()]() {
            return game.delta_time;
        });

        // Scene
        lua.set_function("go_to_scene", goToScene);

        // Entity creation & deletion
        lua.set_function("spawn_entity", [&lua, &registry, &named_entities](const sol::table& entity) -> Entity {
            return SceneLoader::createEntity(lua, entity, registry, named_entities);
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

    void update(sol::state& lua) {
        for (auto entity : getSystemEntities()) {
            const auto& script = entity.getComponent<ScriptComponent>();
            if (script.update.valid()) {
                lua["this"] = entity;
                script.update();
            }
        }
    }
};

#endif // SCRIPT_SYSTEM_HPP