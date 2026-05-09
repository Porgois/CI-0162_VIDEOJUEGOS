#ifndef MOUSE_FOLLOW_SYSTEM_HPP
#define MOUSE_FOLLOW_SYSTEM_HPP

#include <cmath>
#include <SDL2/SDL.h>

#include "../e.c.s./ecs.hpp"
#include "../components/transformComponent.hpp"
#include "../components/mouseFollowComponent.hpp"
#include "../components/spriteComponent.hpp"
#include "../components/childOfComponent.hpp"

class MouseFollowSystem : public System {
    public:
        MouseFollowSystem() {
            requireComponent<TransformComponent>();
            requireComponent<MouseFollowComponent>();
            requireComponent<SpriteComponent>();
            requireComponent<ChildOfComponent>();
        }

        void update(SDL_Rect& camera, float zoom_level) {
            for (auto& entity : getSystemEntities()) {
                auto& transform  = entity.getComponent<TransformComponent>();
                auto& mouse_follow = entity.getComponent<MouseFollowComponent>();
                auto& sprite = entity.getComponent<SpriteComponent>();
                auto& child_of = entity.getComponent<ChildOfComponent>();
                
                auto& parent_transform = child_of.parent.getComponent<TransformComponent>();
                auto& parent_sprite = child_of.parent.getComponent<SpriteComponent>();

                float cx = (parent_transform.position.x + parent_sprite.width  / 2) * zoom_level - camera.x;
                float cy = (parent_transform.position.y + parent_sprite.height / 2) * zoom_level - camera.y;

                if (mouse_follow.is_active) {
                    int mouse_x, mouse_y;
                    SDL_GetMouseState(&mouse_x, &mouse_y);

                    // Update and cache the angle only while active
                    mouse_follow.last_angle = atan2f(mouse_y - cy, mouse_x - cx) * (180.0f / M_PI);

                    // Flip & rotation
                    float deadzone = 0.15f;
                    if (mouse_x > cx + deadzone) {
                        sprite.flip = SDL_FLIP_NONE;
                        transform.rotation = mouse_follow.last_angle;
                    } else if (mouse_x < cx - deadzone) {
                        sprite.flip = SDL_FLIP_HORIZONTAL;
                        float mirrored = atan2f(-(mouse_y - cy), mouse_x - cx) * (180.0f / M_PI);
                        transform.rotation = 180.0f - mirrored;
                    }

                    // Dynamic z-index
                    if (mouse_y < cy) {
                        sprite.z_index = 9;
                    } else {
                        sprite.z_index = 10;
                    }
                }

                // Always reapply position from last_angle (frozen when inactive, live when active)
                float rad = mouse_follow.last_angle * (M_PI / 180.0f);
                transform.position.x = (cx + camera.x) / zoom_level + cosf(rad) * mouse_follow.orbit_radius;
                transform.position.y = (cy + camera.y) / zoom_level + sinf(rad) * mouse_follow.orbit_radius;
            }
        }

        void debugRender(SDL_Renderer* renderer, SDL_Rect& camera, float zoom_level) {
            for (auto& entity : getSystemEntities()) {
                auto& transform  = entity.getComponent<TransformComponent>();
                auto& child_of = entity.getComponent<ChildOfComponent>();
                auto& parent_transform = child_of.parent.getComponent<TransformComponent>();
                auto& parent_sprite = child_of.parent.getComponent<SpriteComponent>();

                // Red dot = rotation origin (pivot)
                float cx = (parent_transform.position.x + parent_sprite.width  * 0.5f) * zoom_level - camera.x;
                float cy = (parent_transform.position.y + parent_sprite.height * 0.5f) * zoom_level - camera.y;
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_Rect center_dot = { (int)cx - 2, (int)cy - 2, 4, 4 };
                SDL_RenderFillRect(renderer, &center_dot);

                // Green dot = follower transform.position
                float rx = transform.position.x * zoom_level - camera.x;
                float ry = transform.position.y * zoom_level - camera.y;
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_Rect rev_dot = { (int)rx - 2, (int)ry - 2, 4, 4 };
                SDL_RenderFillRect(renderer, &rev_dot);
            }
        }

};

#endif // MOUSE_FOLLOW_SYSTEM_HPP