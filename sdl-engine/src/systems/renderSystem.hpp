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
        std::vector<Entity>& tile_entities, bool render_unlit = false,
        SDL_Texture* darkness_texture = nullptr,
        SDL_Texture* scratch_texture = nullptr) {

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
                auto& a_sprite    = a.entity.getComponent<SpriteComponent>();
                auto& b_sprite    = b.entity.getComponent<SpriteComponent>();

                return (a_transform.position.y + a_sprite.height * a_transform.scale.y) <
                    (b_transform.position.y + b_sprite.height * b_transform.scale.y);
            }
            return a.is_tile;
        });

        SDL_BlendMode punch_blend = SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_ZERO,
            SDL_BLENDFACTOR_ONE,
            SDL_BLENDOPERATION_ADD,
            SDL_BLENDFACTOR_ZERO,
            SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            SDL_BLENDOPERATION_ADD
        );

        for (auto& item : queue) {
            if (item.is_tile) {
                if (render_unlit) continue;

                const auto& tile_map = item.entity.getComponent<TileMapComponent>();
                SDL_SetTextureBlendMode(tile_map.baked_texture, SDL_BLENDMODE_BLEND);
                SDL_Rect src = { 0, 0, tile_map.width, tile_map.height };
                SDL_Rect dst = {
                    static_cast<int>(-camera.x),
                    static_cast<int>(-camera.y),
                    static_cast<int>(tile_map.width  * zoom_level),
                    static_cast<int>(tile_map.height * zoom_level)
                };
                SDL_RenderCopy(renderer, tile_map.baked_texture, &src, &dst);

            } else {
                const auto& sprite    = item.entity.getComponent<SpriteComponent>();
                const auto& transform = item.entity.getComponent<TransformComponent>();

                if (sprite.is_unlit != render_unlit) continue;

                SDL_Rect srcRect = sprite.srcRect;
                int w = static_cast<int>(sprite.width  * transform.scale.x * zoom_level);
                int h = static_cast<int>(sprite.height * transform.scale.y * zoom_level);
                SDL_Point center = { w / 2, h / 2 };

                int offset_x = (sprite.pivot.x != 0 || sprite.pivot.y != 0) ? w / 2 : 0;
                int offset_y = (sprite.pivot.x != 0 || sprite.pivot.y != 0) ? h / 2 : 0;

                SDL_Rect dstRect;
                if (sprite.is_ui) {
                    dstRect = {
                        static_cast<int>(transform.position.x) - offset_x,
                        static_cast<int>(transform.position.y) - offset_y,
                        static_cast<int>(sprite.width  * transform.scale.x),
                        static_cast<int>(sprite.height * transform.scale.y)
                    };
                } else {
                    dstRect = {
                        static_cast<int>((transform.position.x * zoom_level) - camera.x) - offset_x,
                        static_cast<int>((transform.position.y * zoom_level) - camera.y) - offset_y,
                        w, h
                    };
                }

                if (sprite.is_lit_only && darkness_texture && scratch_texture) {
                    // Render sprite onto scratch (transparent canvas)
                    SDL_SetRenderTarget(renderer, scratch_texture);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                    SDL_RenderClear(renderer);

                    SDL_Texture* tex = asset_manager->getTexture(sprite.textureId);
                    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
                    SDL_RenderCopyEx(renderer, tex, &srcRect, &dstRect, \
                        transform.rotation, &center, sprite.flip);

                    // Punch darkness into scratch to erase dark areas
                    SDL_SetTextureBlendMode(darkness_texture, punch_blend);
                    SDL_SetTextureAlphaMod(darkness_texture, 255);
                    SDL_RenderCopy(renderer, darkness_texture, nullptr, nullptr);
                    SDL_SetTextureBlendMode(darkness_texture, SDL_BLENDMODE_BLEND);

                    // Blit scratch onto screen at correct z-index position
                    SDL_SetRenderTarget(renderer, nullptr);
                    SDL_SetTextureBlendMode(scratch_texture, SDL_BLENDMODE_BLEND);
                    SDL_RenderCopy(renderer, scratch_texture, nullptr, nullptr);

                } else if (!sprite.is_lit_only) {
                    SDL_RenderCopyEx(renderer, asset_manager->getTexture(sprite.textureId),
                        &srcRect, &dstRect, transform.rotation, &center, sprite.flip);
                }
            }
        }
    }
};
#endif