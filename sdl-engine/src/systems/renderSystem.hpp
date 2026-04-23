#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP
#include <SDL2/SDL.h>
#include "../assetManager/assetManager.hpp"
#include "../components/spriteComponent.hpp"
#include "../components/transformComponent.hpp"
#include "../components/tileMapComponent.hpp"
#include "../components/layerComponent.hpp"
#include "../e.c.s./ecs.hpp"

class RenderSystem : public System {
public:
    RenderSystem() {
        requireComponent<SpriteComponent>();
        requireComponent<TransformComponent>();
    }

    void update(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& asset_manager,
                SDL_Rect& camera, float zoom_level,
                std::vector<Entity>& tile_entities) {

        struct RenderItem {
            int z_index;
            bool is_tile;
            Entity entity;
        };

        std::vector<RenderItem> queue;

        for (auto& e : tile_entities) {
            queue.push_back({ e.getComponent<LayerComponent>().z_index, true, e });
        }

        for (auto& e : getSystemEntities()) {
            queue.push_back({ e.getComponent<SpriteComponent>().z_index, false, e });
        }

        std::sort(queue.begin(), queue.end(), [](const RenderItem& a, const RenderItem& b) {
            if (a.z_index != b.z_index) {
                return a.z_index < b.z_index;
            }

            if (!a.is_tile && !b.is_tile) {
                auto& a_transform = a.entity.getComponent<TransformComponent>();
                auto& b_transform = b.entity.getComponent<TransformComponent>();
                auto& a_sprite = a.entity.getComponent<SpriteComponent>();
                auto& b_sprite = b.entity.getComponent<SpriteComponent>();

                return (a_transform.position.y + a_sprite.height * a_transform.scale.y) < \
                        (b_transform.position.y + b_sprite.height * b_transform.scale.y);
            }
            return a.is_tile;
        });

        for (auto& item : queue) {
            if (item.is_tile) { //* TILES
                const auto& tile_map = item.entity.getComponent<TileMapComponent>();
                SDL_SetTextureBlendMode(tile_map.baked_texture, SDL_BLENDMODE_BLEND); // add this
                SDL_Rect src = { 0, 0, tile_map.width, tile_map.height };
                SDL_Rect dst = {
                    static_cast<int>((-camera.x)),
                    static_cast<int>((-camera.y)),
                    static_cast<int>(tile_map.width  * zoom_level),
                    static_cast<int>(tile_map.height * zoom_level)
                };
                SDL_RenderCopy(renderer, tile_map.baked_texture, &src, &dst);
            } else { //*
                const auto& sprite = item.entity.getComponent<SpriteComponent>();
                const auto& transform = item.entity.getComponent<TransformComponent>();
                SDL_Rect srcRect = sprite.srcRect;
                SDL_Rect dstRect = {
                    static_cast<int>((transform.position.x * zoom_level) - camera.x),
                    static_cast<int>((transform.position.y * zoom_level) - camera.y),
                    static_cast<int>(sprite.width  * transform.scale.x * zoom_level),
                    static_cast<int>(sprite.height * transform.scale.y * zoom_level)
                };
                SDL_Point center = { dstRect.w / 2, dstRect.h / 2 };
                SDL_RenderCopyEx(renderer, asset_manager->getTexture(sprite.textureId),
                    &srcRect, &dstRect, transform.rotation, &center, sprite.flip);
            }
        }
    }
};
#endif