#ifndef LUA_BINDING_HPP
#define LUA_BINDING_HPP

#include <string>
#include <iostream>
#include <sol/sol.hpp>
#include <vector>

#include "../systems/cameraMovementSystem.hpp"

#include "../game/game.hpp"
#include "../e.c.s./ecs.hpp"
#include "../sceneManager/sceneLoader.hpp"
#include "../components/rigidBodyComponent.hpp"
#include "../components/flashlightComponent.hpp"
#include "../components/tagComponent.hpp"
#include "../components/animationComponent.hpp"
#include "../components/boxColliderComponent.hpp"
#include "../components/cameraFollowComponent.hpp"
#include "../components/mouseFollowComponent.hpp"
#include "../components/transformComponent.hpp"
#include "../components/spriteComponent.hpp"

//* Controls
bool isActionActive(const std::string& action) {
    return Game::getInstance().controller_manager->isActionActive(action);
}

//* Mouse
bool isButtonPressed(const std::string& name) {
    return Game::getInstance().controller_manager->isMouseButtonDown(name);
}

bool isButtonJustPressed(const std::string& name) {
    return Game::getInstance().controller_manager->isMouseButtonJustPressed(name);
}

//* Rigidbody
void setVelocity(Entity entity, float velocity_x, float velocity_y) {
    if (!entity.hasComponent<RigidBodyComponent>()) {
        return;
    }
    auto& rigidbody = entity.getComponent<RigidBodyComponent>();
    rigidbody.velocity.x = velocity_x;
    rigidbody.velocity.y = velocity_y;
}

std::tuple<int, int> getVelocity(Entity entity) {
    if (!entity.hasComponent<RigidBodyComponent>()) {
        return {0, 0};
    }
    auto& rigidbody = entity.getComponent<RigidBodyComponent>();
    return {
        static_cast<int>(rigidbody.velocity.x),
        static_cast<int>(rigidbody.velocity.y)
    };
}

//* Setters
void setAnimation(Entity entity, const std::string animation_name) {
    if (!entity.hasComponent<AnimationComponent>()) {
        return;
    }
    auto& animation = entity.getComponent<AnimationComponent>();
    animation.play(animation_name);
}

void setPosition(Entity entity, double x, double y) {
    if (!entity.hasComponent<TransformComponent>()) {
        return;
    }
    auto& transform = entity.getComponent<TransformComponent>();

    transform.position.x = x;
    transform.position.y = y;
}

void setRotation(Entity entity, double rotation) {
    if (!entity.hasComponent<TransformComponent>()) {
        return;
    }
    auto& transform = entity.getComponent<TransformComponent>();

    transform.rotation = rotation;
}

//* Getters
std::tuple<int, int> getPosition(Entity entity) {
    if (!entity.hasComponent<TransformComponent>()) {
        return {0, 0};
    }
    const auto& transform = entity.getComponent<TransformComponent>();
    return {
        static_cast<int>(transform.position.x),
        static_cast<int>(transform.position.y)
    };
}

std::tuple<float, float> getPivotedPosition(Entity entity) {
    if (!entity.hasComponent<TransformComponent>() || !entity.hasComponent<SpriteComponent>()) {
        return {0.0f, 0.0f};
    }
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
    if (!entity.hasComponent<TransformComponent>()) {
        return {0, 0};
    }
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

std::tuple<int, int> getMousePosition() {
    auto [mouse_x, mouse_y] = Game::getInstance().controller_manager->getMousePosition();
 
    return {
        static_cast<int>(mouse_x),
        static_cast<int>(mouse_y)
    };
}

std::tuple<int, int> getColliderSize(Entity entity) { 
    if (!entity.hasComponent<BoxColliderComponent>() || !entity.hasComponent<TransformComponent>()) {
        return {0, 0};
    }
    auto& collider = entity.getComponent<BoxColliderComponent>();
    auto& transform = entity.getComponent<TransformComponent>();

    int width = collider.width * transform.scale.x;
    int height = collider.height * transform.scale.y;

    return {width, height};
}

std::tuple<int, int> getColliderOffset(Entity entity) { 
    if (!entity.hasComponent<BoxColliderComponent>() || !entity.hasComponent<TransformComponent>()) {
        return {0, 0};
    }
    auto& collider = entity.getComponent<BoxColliderComponent>();
    auto& transform = entity.getComponent<TransformComponent>();
    return {
        static_cast<int>(collider.offset.x * transform.scale.x),
        static_cast<int>(collider.offset.y * transform.scale.y)
    };
}

bool getFlip(Entity entity) {
    if (!entity.hasComponent<SpriteComponent>()) {
        return false;
    }
    auto& sprite = entity.getComponent<SpriteComponent>();
    return sprite.flip == SDL_FLIP_HORIZONTAL;
}

//* Audio
void playAudio(const std::string& audio_path, int loops, int volume) {
    Game::getInstance().audio_manager->playSound(audio_path, loops, volume);
}

void playMusic(const std::string& music_path, int loops, int volume) {
    Game::getInstance().audio_manager->playMusic(music_path, loops, volume);
}

void playRandomAudio(const std::vector<std::string>& audio_paths, int loops, int volume) {
    Game::getInstance().audio_manager->playRandomSound(audio_paths, loops, volume);
}

void stopMusic() {
    Game::getInstance().audio_manager->stopMusic();
}

//* Scene switching
void goToScene(const std::string& scene_name, bool fade = false, float hold_duration = -1.0f) {
    Game& game = Game::getInstance();
    if (fade && hold_duration >= 0.0f) {
        game.scene_transition_hold_duration = hold_duration;
    }
    game.scene_manager->setNextScene(scene_name);
    if (fade) {
        game.requestSceneTransition();
    } else {
        game.scene_manager->stopScene();
    }
}

//* Camera follow
void toggleCameraFollow(Entity entity, bool value) {
    if (!entity.hasComponent<CameraFollowComponent>()) {
        return;
    }
    auto& camera_follow = entity.getComponent<CameraFollowComponent>();
    camera_follow.is_active = value;
}

//* Mouse follow
void toggleMouseFollow(Entity entity, bool value) {
    if (!entity.hasComponent<MouseFollowComponent>()) {
        return;
    }
    auto& mouse_follow = entity.getComponent<MouseFollowComponent>();
    mouse_follow.is_active = value;
}

//* Flashlight
void toggleFlashlight(Entity entity, bool value) {
    if (!entity.hasComponent<FlashlightComponent>()) {
        return;
    }
    auto& flashlight = entity.getComponent<FlashlightComponent>();
    if (!value){ // Turn on
        flashlight.mode = FlashlightMode::CircleOnly;
    } else { // Turn off
        flashlight.mode = FlashlightMode::Full;
    }
}

//* Sprite 
void toggleSpriteFlip(Entity entity, bool value) {
    if (!entity.hasComponent<SpriteComponent>()) {
        return;
    }
    auto& sprite = entity.getComponent<SpriteComponent>();
    sprite.flip_to_mouse = value;
}

//* Tag
std::string getTag(Entity entity) {
    if (!entity.hasComponent<TagComponent>()) {
        return "";
    }
    return entity.getComponent<TagComponent>().tag;
}

//* Entity management
Entity findEntity(std::string entity_name) {
    return Game::getInstance().registry->findEntity(entity_name);
}

bool hasEntity(std::string entity_name) {
    return Game::getInstance().registry->hasEntity(entity_name);
}

//* Component alteration
void removeBoxCollider(Entity entity) {
    entity.removeComponent<BoxColliderComponent>();
}

//* Misc
void shakeCamera(float duration, float intensity, float frequency) {
    Game::getInstance().registry->getSystem<CameraMovementSystem>()
        .shake(duration, intensity, frequency);
}

//* Text
void setTextText(Entity entity, std::string new_text = "") {
    if (entity.hasComponent<TextComponent>()) {
        entity.getComponent<TextComponent>().text = new_text;
    } else {
        std::cerr << "[LUA BINDING] Error: Entity has no text component!\n" << std::endl;
    }
    
}

//* Animation
bool isAnimationFinished(Entity entity) {
    if (!entity.hasComponent<AnimationComponent>()) {
        return false;
    }

    auto& animation = entity.getComponent<AnimationComponent>();
    return animation.isFinished();
}
    
#endif // LUA_BINDING_HPP