#ifndef LUA_BINDING_HPP
#define LUA_BINDING_HPP

#include <string>

#include "../game/game.hpp"
#include "../components/rigidBodyComponent.hpp"
#include "../components/animationComponent.hpp"
#include "../e.c.s./ecs.hpp"

// Controls
bool isActionActive(const std::string& action) {
    return Game::getInstance().controller_manager->isActionActive(action);
}

// Rigidbody
void setVelocity(Entity entity, float velocity_x, float velocity_y) {
    auto& rigidbody = entity.getComponent<RigidBodyComponent>();
    rigidbody.velocity.x = velocity_x;
    rigidbody.velocity.y = velocity_y;
}

// Animation
void setAnimation(Entity entity, const std::string animation_name) {
    auto& animation = entity.getComponent<AnimationComponent>();
    animation.play(animation_name);
}

// Scene switching
void goToScene(const std::string& scene_name) {
    Game::getInstance().scene_manager->setNextScene(scene_name);
    Game::getInstance().scene_manager->stopScene();
}

#endif // LUA_BINDING_HPP