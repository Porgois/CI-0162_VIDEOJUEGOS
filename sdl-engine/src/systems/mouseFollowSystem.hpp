#ifndef MOUSE_FOLLOW_SYSTEM_HPP
#define MOUSE_FOLLOW_SYSTEM_HPP

#include <cmath>
#include <SDL2/SDL.h>

#include "../e.c.s./ecs.hpp"
#include "../components/transformComponent.hpp"
#include "../components/mouseFollowComponent.hpp"
#include "../components/spriteComponent.hpp"

class MouseFollowSystem : public System {
    public:
        MouseFollowSystem() {
            requireComponent<TransformComponent>();
            requireComponent<MouseFollowComponent>();
            requireComponent<SpriteComponent>();
        }

        void update(SDL_Rect& camera, float zoom_level) {
            int mouse_x, mouse_y;
            SDL_GetMouseState(&mouse_x, &mouse_y);

            for (auto& entity : getSystemEntities()) {
                auto& transform = entity.getComponent<TransformComponent>();
                auto& mouse_follow = entity.getComponent<MouseFollowComponent>();
                auto& sprite = entity.getComponent<SpriteComponent>();
                
                // transform.position IS already the hand position, set by ChildOfSystem
                float cx = transform.position.x * zoom_level - camera.x;
                float cy = transform.position.y * zoom_level - camera.y;

                float angle = atan2f(mouse_y - cy, mouse_x - cx) * (180.0f / M_PI);

                float deadzone = 5.0f;
                if (mouse_x < cx - deadzone) {
                    sprite.flip = SDL_FLIP_HORIZONTAL;
                    float mirrored = atan2f(-(mouse_y - cy), mouse_x - cx) * (180.0f / M_PI);
                    transform.rotation = 180.0f - mirrored;
                } else if (mouse_x > cx + deadzone) {
                    sprite.flip = SDL_FLIP_NONE;
                    sprite.pivot.x = mouse_follow.origin_pivot.x;
                    transform.rotation = angle;
                }
            }
        }
};

#endif // MOUSE_FOLLOW_SYSTEM_HPP