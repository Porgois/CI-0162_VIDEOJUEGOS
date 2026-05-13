#ifndef TEXT_RENDER_SYSTEM_HPP
#define TEXT_RENDER_SYSTEM_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>

#include "../e.c.s./ecs.hpp"
#include "../assetManager/assetManager.hpp"
#include "../components/textComponent.hpp"
#include "../components/transformComponent.hpp"

class TextRenderSystem : public System {
    public:
        TextRenderSystem() {
            requireComponent<TextComponent>();
            requireComponent<TransformComponent>();
        }

        void update(SDL_Renderer* renderer, const std::unique_ptr<AssetManager>& asset_manager, SDL_Rect& camera, float zoom_level) {
            for (auto entity : getSystemEntities()) {
                auto& text = entity.getComponent<TextComponent>();
                auto& transform = entity.getComponent<TransformComponent>();

                TTF_Font* font = asset_manager->getFont(text.font_id);
                if (!font) {
                    std::cerr << "[TEXT RENDER] Missing font for id: " << text.font_id << std::endl;
                    continue;
                }

                SDL_Texture* texture = nullptr;

                if (text.outline_thickness > 0) {
                    // Render outline first
                    TTF_SetFontOutline(font, text.outline_thickness);
                    SDL_Surface* outline_surface = TTF_RenderText_Blended(font, text.text.c_str(), text.outline_color);
                    if (!outline_surface) {
                        std::cerr << "[TEXT RENDER] Outline render failed: " << TTF_GetError() << std::endl;
                        TTF_SetFontOutline(font, 0);
                        continue;
                    }

                    // Render inner text on top
                    TTF_SetFontOutline(font, 0);
                    SDL_Surface* fg_surface = TTF_RenderText_Blended(font, text.text.c_str(), text.color);
                    if (!fg_surface) {
                        std::cerr << "[TEXT RENDER] Text render failed: " << TTF_GetError() << std::endl;
                        SDL_FreeSurface(outline_surface);
                        continue;
                    }

                    // Blit fg centered onto outline (offset by thickness)
                    SDL_Rect fg_rect = { text.outline_thickness, text.outline_thickness, fg_surface->w, fg_surface->h };
                    SDL_SetSurfaceBlendMode(fg_surface, SDL_BLENDMODE_BLEND);
                    SDL_BlitSurface(fg_surface, NULL, outline_surface, &fg_rect);
                    SDL_FreeSurface(fg_surface);

                    text.width = outline_surface->w;
                    text.height = outline_surface->h;
                    texture = SDL_CreateTextureFromSurface(renderer, outline_surface);
                    SDL_FreeSurface(outline_surface);
                    if (!texture) {
                        std::cerr << "[TEXT RENDER] Failed to create texture from outline surface: " << SDL_GetError() << std::endl;
                        continue;
                    }
                } else {
                    SDL_Surface* surface = TTF_RenderText_Blended(font, text.text.c_str(), text.color);
                    if (!surface) {
                        std::cerr << "[TEXT RENDER] Text render failed: " << TTF_GetError() << std::endl;
                        continue;
                    }
                    text.width = surface->w;
                    text.height = surface->h;
                    texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_FreeSurface(surface);
                    if (!texture) {
                        std::cerr << "[TEXT RENDER] Failed to create texture from surface: " << SDL_GetError() << std::endl;
                        continue;
                    }
                }

                SDL_Rect dstRect = {
                    static_cast<int>((transform.position.x * zoom_level) - camera.x),
                    static_cast<int>((transform.position.y * zoom_level) - camera.y),
                    text.width  * static_cast<int>(transform.scale.x),
                    text.height * static_cast<int>(transform.scale.y)
                };

                SDL_RenderCopy(renderer, texture, NULL, &dstRect);
                SDL_DestroyTexture(texture);
            }
    }
};
#endif // TEXT_RENDER_SYSTEM_HPP