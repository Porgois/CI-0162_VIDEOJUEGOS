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

        void update(SDL_Renderer* renderer, const std::unique_ptr<AssetManager>& asset_manager) {
            for (auto entity : getSystemEntities()) {
                auto& text = entity.getComponent<TextComponent>();
                auto& transform = entity.getComponent<TransformComponent>();

                SDL_Surface* surface = TTF_RenderText_Blended(asset_manager->getFont(text.font_id), \
                    text.text.c_str(), text.color);

                text.width = surface->w;
                text.height = surface->h;
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);

                SDL_Rect dstRect = {
                    static_cast<int>(transform.position.x),
                    static_cast<int>(transform.position.y),
                    text.width * static_cast<int>(transform.scale.x),
                    text.height * static_cast<int>(transform.scale.y)
                };

                SDL_RenderCopy(renderer, texture, NULL, &dstRect);
                SDL_DestroyTexture(texture);
            }
        }
};
#endif // TEXT_RENDER_SYSTEM_HPP