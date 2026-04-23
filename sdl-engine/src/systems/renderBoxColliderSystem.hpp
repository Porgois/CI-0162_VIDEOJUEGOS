#ifndef RENDER_BOX_COLLIDER_SYSTEM_HPP
#define RENDER_BOX_COLLIDER_SYSTEM_HPP

#include <SDL2/SDL.h>

#include "../components/boxColliderComponent.hpp"
#include "../components/transformComponent.hpp"
#include "../e.c.s./ecs.hpp"

class RenderBoxColliderSystem : public System {
    public:
        RenderBoxColliderSystem() {
            requireComponent<BoxColliderComponent>();
            requireComponent<TransformComponent>();
        }
    
        void update(SDL_Renderer* renderer, SDL_Rect& camera, float zoom_level) {
            for (auto entity : getSystemEntities()) {
                // Get neccesary components
                auto& transform = entity.getComponent<TransformComponent>();
                const auto& collider = entity.getComponent<BoxColliderComponent>();

                SDL_Rect box = {
                    static_cast<int>(((transform.position.x + collider.offset.x) * zoom_level) - camera.x),
                    static_cast<int>(((transform.position.y + collider.offset.y) * zoom_level) - camera.y),
                    static_cast<int>(collider.width * transform.scale.x * zoom_level),
                    static_cast<int>(collider.height * transform.scale.y * zoom_level)
                };

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &box);
            }
        };
};

#endif // RENDER_BOX_COLLIDER__SYSTEM_HPP
