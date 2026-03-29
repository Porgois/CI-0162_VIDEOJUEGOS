#ifndef ANIMATION_SYSTEM_HPP
#define ANIMATION_SYSTEM_HPP

#include <SDL2/SDL.h>

#include "../e.c.s./ecs.hpp"
#include "../components/spriteComponent.hpp"
#include "../components/animationComponent.hpp"

class AnimationSystem : public System {
    public:
        AnimationSystem() {
            requireComponent<AnimationComponent>();
            requireComponent<SpriteComponent>();
        }

        void update() {
            for (auto entity : getSystemEntities()) {
                auto& animation = entity.getComponent<AnimationComponent>();
                auto& sprite = entity.getComponent<SpriteComponent>();

                auto& clip = animation.currentClip();
                int frame = static_cast<int>( (SDL_GetTicks() - animation.start_time) \
                    * clip.speed / 1000.0 ) % clip.frame_count;

                if (!clip.loops && frame == clip.frame_count - 1) {
                    frame = clip.frame_count - 1; // freeze on the last frame
                }

                animation.current_frame = frame;
                sprite.srcRect.x = frame * sprite.width;
                sprite.srcRect.y = clip.row * sprite.height;
            }
        }
};

#endif // ANIMATION_SYSTEM_HPP