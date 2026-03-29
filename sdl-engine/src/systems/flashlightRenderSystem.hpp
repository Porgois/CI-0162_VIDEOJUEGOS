#ifndef FLASLIGHT_RENDER_SYSTEM_HPP
#define FLASLIGHT_RENDER_SYSTEM_HPP

#include <algorithm>
#include <SDL2/SDL.h>
#include <memory>
#include <cstdlib>
#include <cmath>

#include "../assetManager/assetManager.hpp"
#include "../e.c.s./ecs.hpp"
#include "../components/transformComponent.hpp"
#include "../components/flashlightComponent.hpp"

class FlashlightRenderSystem : public System {
    private:
        SDL_Texture* darkness_texture = nullptr;
        // screen width and height
        int screen_w;
        int screen_h;
        // blend mode
        SDL_BlendMode punch_blend;
        int screen_color = 175;

    public:
        FlashlightRenderSystem(SDL_Renderer* renderer, int screen_w, int screen_h) :
            screen_w(screen_w), screen_h(screen_h) {
            // Requirements
            requireComponent<TransformComponent>();
            requireComponent<FlashlightComponent>();

            // Darkness overlay texture (black)
            darkness_texture = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_TARGET, // render target
                screen_w,
                screen_h
            );

            if (!darkness_texture) {
                SDL_Log("darkness_texture failed: %s", SDL_GetError());
            }

            SDL_SetTextureBlendMode(darkness_texture, SDL_BLENDMODE_BLEND);

            punch_blend = SDL_ComposeCustomBlendMode(
                SDL_BLENDFACTOR_ZERO,
                SDL_BLENDFACTOR_ONE,
                SDL_BLENDOPERATION_ADD,
                SDL_BLENDFACTOR_ZERO,
                SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                SDL_BLENDOPERATION_ADD
            );
        }

        ~FlashlightRenderSystem() {
            if (darkness_texture) {
                SDL_DestroyTexture(darkness_texture);
            }
        }
        
        // TODO: Move this somewhere else
        inline float angleToMouse(float cx, float cy, int mouse_x, int mouse_y) {
            SDL_GetMouseState(&mouse_x, &mouse_y);
            return atan2f(mouse_y - cy, mouse_x - cx) * (180.0f / M_PI);
        }

        void update(SDL_Renderer* renderer, \
            std::unique_ptr<AssetManager>& asset_manager) {
            // Render target to darkness texture
            SDL_SetRenderTarget(renderer, darkness_texture);

            // Fill with black
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 240);
            SDL_RenderClear(renderer);

            for (auto& entity : getSystemEntities()) {
                const auto& transform = entity.getComponent<TransformComponent>();
                auto& flashlight = entity.getComponent<FlashlightComponent>();
                
                // Flicker
                if (flashlight.flicker_enabled) {
                    float target = 0.85f + (rand() % 100 / 100.0f) * 0.15f;
                    flashlight.flicker_intensity += (target - flashlight.flicker_intensity)
                        * flashlight.flicker_speed;
                }

                // Distance scale
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                float cx = transform.position.x + flashlight.origin_offset_x;
                float cy = transform.position.y + flashlight.origin_offset_y;

                float dist = sqrtf((mouse_x - cx) * (mouse_x - cx) + (mouse_y - cy) * (mouse_y - cy));
                float t = std::clamp(dist / flashlight.reach, 0.0f, 1.0f);
                float distance_scale = flashlight.min_scale + t * (flashlight.max_scale - flashlight.min_scale);
                float final_scale = distance_scale * flashlight.flicker_intensity;

                float angle = angleToMouse(cx, cy, mouse_x, mouse_y);

                Uint8 darkness_alpha = static_cast<Uint8>(screen_color * final_scale);
                SDL_SetTextureAlphaMod(darkness_texture, darkness_alpha);

                // light circle
                SDL_Texture* circle_texture = asset_manager->getTexture(flashlight.circle_texture_id);
                SDL_SetTextureBlendMode(circle_texture, punch_blend);
                
                int flicker_radius = static_cast<int>(flashlight.source_radius * flashlight.flicker_intensity);
                SDL_Rect circle_dst = {
                    static_cast<int>(cx) - flicker_radius,
                    static_cast<int>(cy) - flicker_radius,
                    flicker_radius * 2,
                    flicker_radius * 2
                };

                if (flashlight.is_on) { // skip if off
                    // Light cone
                    SDL_Point cone_origin = {
                        static_cast<int>(flashlight.cone_origin_x * final_scale),
                        static_cast<int>(flashlight.cone_origin_y * final_scale)
                    };

                    float cone_end_offset = flashlight.cone_end_offset * (dist / flashlight.reach);

                    SDL_Rect cone_dst = {
                        static_cast<int>(cx) - static_cast<int>(flashlight.cone_origin_x * final_scale),
                        static_cast<int>(cy) - static_cast<int>(flashlight.cone_origin_y * final_scale),
                        static_cast<int>(dist + cone_end_offset),  // dynamic width = exact distance to mouse
                        static_cast<int>(flashlight.cone_height * final_scale)
                    };
                    SDL_Texture* cone_texture = asset_manager->getTexture(flashlight.cone_texture_id);
                    SDL_SetTextureBlendMode(cone_texture, punch_blend);
                    SDL_RenderCopyEx(renderer, cone_texture, nullptr, &cone_dst, angle,
                        &cone_origin, SDL_FLIP_NONE);
                }
                
                SDL_RenderCopy(renderer, circle_texture, nullptr, &circle_dst);               
            }

            // Reset render targets
            SDL_SetRenderTarget(renderer, nullptr);
            SDL_SetTextureAlphaMod(darkness_texture, screen_color); // reset to screen_color
            SDL_RenderCopy(renderer, darkness_texture, nullptr, nullptr);
        }
};

#endif // FLASLIGHT_RENDER_SYSTEM_HPP