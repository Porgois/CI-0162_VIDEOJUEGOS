#ifndef RENDER_BOX_COLLIDER_SYSTEM_HPP
#define RENDER_BOX_COLLIDER_SYSTEM_HPP

#include <cmath>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

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
                auto& transform = entity.getComponent<TransformComponent>();
                const auto& collider = entity.getComponent<BoxColliderComponent>();

                float rad = glm::radians(transform.rotation);
                float cosR = std::cos(rad);
                float sinR = std::sin(rad);

                float w = collider.width  * transform.scale.x * zoom_level;
                float h = collider.height * transform.scale.y * zoom_level;

                glm::vec2 offset_scaled = {
                    collider.offset.x * transform.scale.x * zoom_level,
                    collider.offset.y * transform.scale.y * zoom_level
                };

                glm::vec2 center = {
                    (transform.position.x * zoom_level) - camera.x + (offset_scaled.x * cosR - offset_scaled.y * sinR) \
                        + w * 0.5f,
                    (transform.position.y * zoom_level) - camera.y + (offset_scaled.x * sinR + offset_scaled.y * cosR) \
                        + h * 0.5f
                };

                glm::vec2 corners[4] = {
                    { -w * 0.5f, -h * 0.5f },
                    {  w * 0.5f, -h * 0.5f },
                    {  w * 0.5f,  h * 0.5f },
                    { -w * 0.5f,  h * 0.5f }
                };

                // Rotate each corner and draw lines between them
                glm::vec2 rotated[4];
                for (int i = 0; i < 4; i++) {
                    rotated[i] = {
                        center.x + corners[i].x * cosR - corners[i].y * sinR,
                        center.y + corners[i].x * sinR + corners[i].y * cosR
                    };
                }

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                for (int i = 0; i < 4; i++) {
                    glm::vec2& a = rotated[i];
                    glm::vec2& b = rotated[(i + 1) % 4];
                    SDL_RenderDrawLine(renderer,
                        static_cast<int>(a.x), static_cast<int>(a.y),
                        static_cast<int>(b.x), static_cast<int>(b.y)
                    );
                }
            }
        }
};

#endif // RENDER_BOX_COLLIDER__SYSTEM_HPP
