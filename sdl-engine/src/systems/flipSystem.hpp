#ifndef FLIP_SYSTEM_HPP
#define FLIP_SYSTEM_HPP

#include <SDL2/SDL.h>
#include "../e.c.s./ecs.hpp"
#include "../components/spriteComponent.hpp"
#include "../components/transformComponent.hpp"

class FlipSystem : public System {
    public:
        FlipSystem() {
            requireComponent<SpriteComponent>();
            requireComponent<TransformComponent>();
        }

        void update(SDL_Rect& camera) {
            for (auto entity : getSystemEntities()) {
                auto& sprite = entity.getComponent<SpriteComponent>();
                const auto& transform = entity.getComponent<TransformComponent>();

                if (!sprite.flip_to_mouse) continue;

                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);

                float center_x = (transform.position.x - camera.x)+ (sprite.width * transform.scale.x) / 2.0f;
                sprite.flip = (mouse_x < center_x) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            }
        }
};
#endif // FLIP_SYSTEM_HPP