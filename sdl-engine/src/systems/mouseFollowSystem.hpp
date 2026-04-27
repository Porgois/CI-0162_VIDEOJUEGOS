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
            int mouse_x, mouse_y;
            SDL_GetMouseState(&mouse_x, &mouse_y);

            for (auto& entity : getSystemEntities()) {
                auto& transform = entity.getComponent<TransformComponent>();
                auto& mouse_follow = entity.getComponent<MouseFollowComponent>();
                auto& sprite = entity.getComponent<SpriteComponent>();
                auto& child_of = entity.getComponent<ChildOfComponent>();

                auto& parent_transform = child_of.parent.getComponent<TransformComponent>();
                auto& parent_sprite = child_of.parent.getComponent<SpriteComponent>();
                float cx = (parent_transform.position.x + parent_sprite.width / 2) * zoom_level - camera.x;
                float cy = (parent_transform.position.y + parent_sprite.height / 2) * zoom_level - camera.y;

                float angle = atan2f(mouse_y - cy, mouse_x - cx) * (180.0f / M_PI);
                float orbit_radius = mouse_follow.orbit_radius;
                float rad = angle * (M_PI / 180.0f);
                float deadzone = 0.15f;

                // Position
                transform.position.x = (cx + camera.x) / zoom_level + cosf(rad) * orbit_radius;
                transform.position.y = (cy + camera.y) / zoom_level + sinf(rad) * orbit_radius;

                // Flip & rotation
                if (mouse_x > cx + deadzone) {
                    sprite.flip = SDL_FLIP_NONE;
                    transform.rotation = angle;
                } else if (mouse_x < cx - deadzone) {
                    sprite.flip = SDL_FLIP_HORIZONTAL;
                    float mirrored = atan2f(-(mouse_y - cy), mouse_x - cx) * (180.0f / M_PI);
                    transform.rotation = 180.0f - mirrored;
                }

                // Dynamic z-index
                if (mouse_y < cy) {
                    sprite.z_index = 9;
                } else if (mouse_y > cy) {
                    sprite.z_index = 10;
                }
            }
        }

        void debugRender(SDL_Renderer* renderer, SDL_Rect& camera, float zoom_level) {
            for (auto& entity : getSystemEntities()) {
                auto& transform        = entity.getComponent<TransformComponent>();
                auto& child_of         = entity.getComponent<ChildOfComponent>();
                auto& parent_transform = child_of.parent.getComponent<TransformComponent>();
                auto& parent_sprite    = child_of.parent.getComponent<SpriteComponent>();

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