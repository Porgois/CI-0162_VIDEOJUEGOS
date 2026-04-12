#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include <SDL2/SDL.h>

#include "../assetManager/assetManager.hpp"
#include "../components/spriteComponent.hpp"
#include "../components/transformComponent.hpp"
#include "../e.c.s./ecs.hpp"

class RenderSystem : public System {
    public:
        RenderSystem() {
            // Requires both a sprite and a transform
            requireComponent<SpriteComponent>();
            requireComponent<TransformComponent>();
        }

        void ySort(std::vector<Entity>& entities) {
            std::sort(entities.begin(), entities.end(), [](Entity a, Entity b) {
                auto& a_transform = a.getComponent<TransformComponent>();
                auto& b_transform = b.getComponent<TransformComponent>();
                auto& a_sprite = a.getComponent<SpriteComponent>();
                auto& b_sprite = b.getComponent<SpriteComponent>();

                float a_base = a_transform.position.y + (a_sprite.height * a_transform.scale.y);
                float b_base = b_transform.position.y + (b_sprite.height * b_transform.scale.y);
                
                if (a_sprite.z_index != b_sprite.z_index) { // take z_index into account when sorting
                    return a_sprite.z_index < b_sprite.z_index;
                }

                return a_base < b_base;
            }); 
        }

        void update(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& asset_manager, SDL_Rect& camera) {
            auto entities = getSystemEntities();

            //sort by y position before render
            ySort(entities);

            for (auto entity : entities) {
                const auto& sprite = entity.getComponent<SpriteComponent>();
                const auto& transform = entity.getComponent<TransformComponent>();

                SDL_Rect srcRect = sprite.srcRect;

                // Create rendered element
                SDL_Rect dstRect = {
                    static_cast<int>(transform.position.x - camera.x), // x adjust for camera follow
                    static_cast<int>(transform.position.y - camera.y), // y adjust for camera follow
                    static_cast<int>(sprite.width * transform.scale.x),
                    static_cast<int>(sprite.height * transform.scale.y)
                };

                SDL_RendererFlip flip = sprite.flip;
                SDL_Point center = { dstRect.w / 2, dstRect.h / 2 };

                SDL_RenderCopyEx(
                    renderer,
                    asset_manager->getTexture(sprite.textureId),
                    &srcRect,
                    &dstRect,
                    transform.rotation,
                    &center,
                    flip
                );
            }
        }
};

#endif // RENDER_SYSTEM_HPP