#ifndef SCRIPT_COMPONENT_HPP
#define SCRIPT_COMPONENT_HPP

#include <sol/sol.hpp>

struct ScriptComponent {
    sol::function update;
    sol::function start;
    sol::function onClick;
    sol::function onCollision;
    sol::environment env;
    bool started = false;

    ScriptComponent() = default;

    ScriptComponent(sol::function update, sol::function start, 
        sol::function onClick, sol::function onCollision, sol::environment env)
            : update(update), start(start), onClick(onClick), onCollision(onCollision), env(env), started(false) {}
};

#endif // SCRIPT_COMPONENT_HPP