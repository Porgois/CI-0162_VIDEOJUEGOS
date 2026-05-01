#ifndef LUA_BINDING_HPP
#define LUA_BINDING_HPP

#include <string>
#include <sol/sol.hpp>

#include "../game/game.hpp"
#include "../sceneManager/sceneLoader.hpp"
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

// Mouse
bool isButtonPressed(const std::string& name) {
    return Game::getInstance().controller_manager->isMouseButtonDown(name);
}

bool isButtonJustPressed(const std::string& name) {
    return Game::getInstance().controller_manager->isMouseButtonJustPressed(name);
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

//* Setters
void setAnimation(Entity entity, const std::string animation_name) {
    auto& animation = entity.getComponent<AnimationComponent>();
    animation.play(animation_name);
}

void setPosition(Entity entity, double x, double y) {
    auto& transform = entity.getComponent<TransformComponent>();

    transform.position.x = x;
    transform.position.y = y;
}

void setRotation(Entity entity, double rotation) {
    auto& transform = entity.getComponent<TransformComponent>();

    transform.rotation = rotation;
}

//* Getters
std::tuple<int, int> getPosition(Entity entity) {
    const auto& transform = entity.getComponent<TransformComponent>();
    return {
        static_cast<int>(transform.position.x),
        static_cast<int>(transform.position.y)
    };
}

std::tuple<float, float> getPivotedPosition(Entity entity) {
    const auto& transform = entity.getComponent<TransformComponent>();
    const auto& sprite = entity.getComponent<SpriteComponent>();
    bool has_pivot = (sprite.pivot.x != 0 || sprite.pivot.y != 0);
    float offset_x = has_pivot ? (sprite.width  * transform.scale.x) / 2.0f : 0.0f;
    float offset_y = has_pivot ? (sprite.height * transform.scale.y) / 2.0f : 0.0f;
    return {
        transform.position.x - offset_x,
        transform.position.y - offset_y
    };
}

std::tuple<int, int> getPreviousPosition(Entity entity) {
    const auto& transform = entity.getComponent<TransformComponent>();
    return {
        static_cast<int>(transform.previous_position.x),
        static_cast<int>(transform.previous_position.y)
    };
}

std::tuple<int, int> getMouseWorldPosition() {
    auto [mouse_x, mouse_y] = Game::getInstance().controller_manager->getMousePosition();
    const auto& camera = Game::getInstance().camera;
    const float zoom = Game::getInstance().zoom_level;

    return {
        static_cast<int>((mouse_x + camera.x) / zoom),
        static_cast<int>((mouse_y + camera.y) / zoom)
    };
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

bool getFlip(Entity entity) {
    auto& sprite = entity.getComponent<SpriteComponent>();
    return sprite.flip == SDL_FLIP_HORIZONTAL;
}

//* Scene switching
void goToScene(const std::string& scene_name) {
    Game::getInstance().scene_manager->setNextScene(scene_name);
    Game::getInstance().scene_manager->stopScene();
}

//* Tag
std::string getTag(Entity entity) {
    if (!entity.hasComponent<TagComponent>()) {
        return "";
    }
    return entity.getComponent<TagComponent>().tag;
}
#endif // LUA_BINDING_HPP