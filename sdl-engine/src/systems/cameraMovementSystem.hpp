#ifndef CAMERA_MOVEMENT_SYSTEM_HPP
#define CAMERA_MOVEMENT_SYSTEM_HPP


#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

#include "../e.c.s./ecs.hpp"
#include "../game/game.hpp"
#include "../components/transformComponent.hpp"
#include "../components/cameraFollowComponent.hpp" 

class CameraMovementSystem : public System {
    private:
        float mouse_threshold = 150.0f;
        float mouse_influence = 0.3f;
        float lerp_speed = 0.15f;

        // Screen shake
        float shake_duration = 0.0f;
        float shake_intensity = 0.0f;
        float shake_frequency = 0.0f;
        float shake_timer = 0.0f;

        void applyShake(SDL_Rect& camera, float delta_time, int max_cam_x, int max_cam_y) {
            if (shake_timer >= shake_duration)  {
                return;
            }

            shake_timer += delta_time;

            // Intensity falloff
            float progress  = shake_timer / shake_duration;
            float falloff   = 1.0f - progress;
            float current_intensity = shake_intensity * falloff;

            // offsets based on sin and cos for x and y respectively
            float angle = shake_timer * shake_frequency;
            int offset_x = static_cast<int>(std::sin(angle)        * current_intensity);
            int offset_y = static_cast<int>(std::cos(angle * 1.3f) * current_intensity); // 1.3 avoids perfect circles

            camera.x = std::clamp(camera.x + offset_x, 0, std::max(0, max_cam_x));
            camera.y = std::clamp(camera.y + offset_y, 0, std::max(0, max_cam_y));
        }

    public:
        CameraMovementSystem() {
            requireComponent<TransformComponent>();
            requireComponent<CameraFollowComponent>();
        }

        void shake(float duration, float intensity, float frequency = 30.0f) {
            shake_duration = duration;
            shake_intensity = intensity;
            shake_frequency = frequency;
            shake_timer = 0.0f;
        }

        void update(SDL_Rect& camera, float zoom_level, float delta_time) {
            for (auto& entity : getSystemEntities()) {
                const auto& camera_follow = entity.getComponent<CameraFollowComponent>();
                const auto& transform = entity.getComponent<TransformComponent>();

                if (!camera_follow.is_active) return;

                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);

                float screen_center_x = camera.w / 2.0f;
                float screen_center_y = camera.h / 2.0f;
                float raw_offset_x = mouse_x - screen_center_x;
                float raw_offset_y = mouse_y - screen_center_y;

                float distance = std::sqrt(raw_offset_x * raw_offset_x + raw_offset_y * raw_offset_y);
                float clamped_x = raw_offset_x;
                float clamped_y = raw_offset_y;

                if (distance > mouse_threshold) {
                    float scale = mouse_threshold / distance;
                    clamped_x *= scale;
                    clamped_y *= scale;
                }

                float target_x = transform.position.x * zoom_level;
                float target_y = transform.position.y * zoom_level;
                float desired_x = target_x + (clamped_x * mouse_influence) - screen_center_x;
                float desired_y = target_y + (clamped_y * mouse_influence) - screen_center_y;

                if (transform.position.x + screen_center_x < static_cast<float>(Game::getInstance().map_width)) {
                    camera.x = static_cast<int>(camera.x + (desired_x - camera.x) * lerp_speed);
                }
                if (transform.position.y + screen_center_y < static_cast<float>(Game::getInstance().map_height)) {
                    camera.y = static_cast<int>(camera.y + (desired_y - camera.y) * lerp_speed);
                }

                int max_cam_x = static_cast<int>(Game::getInstance().map_width  * zoom_level) - camera.w;
                int max_cam_y = static_cast<int>(Game::getInstance().map_height * zoom_level) - camera.h;
                camera.x = std::clamp(camera.x, 0, std::max(0, max_cam_x));
                camera.y = std::clamp(camera.y, 0, std::max(0, max_cam_y));

                applyShake(camera, delta_time, max_cam_x, max_cam_y);
            }
        }


};

#endif // CAMERA_MOVEMENT_SYSTEM_HPP