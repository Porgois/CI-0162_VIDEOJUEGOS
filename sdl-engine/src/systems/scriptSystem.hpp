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
        
        void createLuaBinding(sol::state& lua) {
            // classes
            lua.new_usertype<Entity>("entity");

            // functions
            lua.set_function("is_action_active", isActionActive);
            lua.set_function("set_velocity", setVelocity);
            lua.set_function("play_animation", setAnimation);
            lua.set_function("go_to_scene", goToScene);
        }

        void update(sol::state& lua) {
           for (auto entity : getSystemEntities()) {
                const auto& script = entity.getComponent<ScriptComponent>();
                
                if (script.update.valid()) {
                    lua["this"] = entity;
                    script.update();
                }
           }
        };
};

#endif // SCRIPT_SYSTEM_HPP