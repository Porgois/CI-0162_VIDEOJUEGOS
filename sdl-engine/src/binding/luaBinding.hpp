#ifndef LUA_BINDING_HPP
#define LUA_BINDING_HPP

#include <string>

#include "../game/game.hpp"
#include "../components/rigidBodyComponent.hpp"
#include "../components/tagComponent.hpp"
#include "../components/animationComponent.hpp"
#include "../components/boxColliderComponent.hpp"
#include "../components/transformComponent.hpp"
#include "../components/spriteComponent.hpp"
#include "../e.c.s./ecs.hpp"

//* Controls
bool isActionActive(const std::string& action) {
    return Game::getInstance().controller_manager->isActionActive(action);
}

//* Rigidbody
void setVelocity(Entity entity, float velocity_x, float velocity_y) {
    auto& rigidbody = entity.getComponent<RigidBodyComponent>();
    rigidbody.velocity.x = velocity_x;
    rigidbody.velocity.y = velocity_y;
}

std::tuple<int, int> getVelocity(Entity entity) {
    auto& rigidbody = entity.getComponent<RigidBodyComponent>();
    return {
        static_cast<int>(rigidbody.velocity.x),
        static_cast<int>(rigidbody.velocity.y)
    };
}

//* Animation
void setAnimation(Entity entity, const std::string animation_name) {
    auto& animation = entity.getComponent<AnimationComponent>();
    animation.play(animation_name);
}

//* Transform
std::tuple<int, int> getPosition(Entity entity) {
    const auto& transform = entity.getComponent<TransformComponent>();
    return {
        static_cast<int>(transform.position.x),
        static_cast<int>(transform.position.y)
    };
}

std::tuple<int, int> getPreviousPosition(Entity entity) {
    const auto& transform = entity.getComponent<TransformComponent>();
    return {
        static_cast<int>(transform.previous_position.x),
        static_cast<int>(transform.previous_position.y)
    };
}

void setPosition(Entity entity, int x, int y) {
    auto& transform = entity.getComponent<TransformComponent>();

    transform.position.x = x;
    transform.position.y = y;
}

std::tuple<int, int> getColliderSize(Entity entity) { 
    auto& collider = entity.getComponent<BoxColliderComponent>();
    auto& transform = entity.getComponent<TransformComponent>();

    int width = collider.width * transform.scale.x;
    int height = collider.height * transform.scale.y;

    return {width, height};
}

std::tuple<int, int> getColliderOffset(Entity entity) { 
    auto& collider = entity.getComponent<BoxColliderComponent>();
    auto& transform = entity.getComponent<TransformComponent>();
    return {
        static_cast<int>(collider.offset.x * transform.scale.x),
        static_cast<int>(collider.offset.y * transform.scale.y)
    };
}

//* Scene switching
void goToScene(const std::string& scene_name) {
    Game::getInstance().scene_manager->setNextScene(scene_name);
    Game::getInstance().scene_manager->stopScene();
}

//* Tag
std::string getTag(Entity entity) {
    return entity.getComponent<TagComponent>().tag;
}

//* Entity spawn/deleteion
void createEntity(Entity entity) {
    
}

void deleteEntity(Entity entity) {

}

#endif // LUA_BINDING_HPP