#ifndef CURSOR_SYSTEM_HPP
#define CURSOR_SYSTEM_HPP

#include <SDL2/SDL.h>

#include "../e.c.s./ecs.hpp"
#include "../assetManager/assetManager.hpp"
#include "../components/spriteComponent.hpp"
#include "../components/cursorComponent.hpp"
#include "../components/transformComponent.hpp"

class CursorSystem : public System {
    public:
        CursorSystem() {
            requireComponent<CursorComponent>();
            requireComponent<SpriteComponent>();
           SDL_ShowCursor(SDL_DISABLE); // hide system cursor
        }

        ~CursorSystem() {
           SDL_ShowCursor(SDL_ENABLE); // show system cursor
        }

        void update(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& asset_manager) {
            int mouse_x;
            int mouse_y;
            SDL_GetMouseState(&mouse_x, &mouse_y);

            for (auto& entity : getSystemEntities()) {
                const auto& sprite = entity.getComponent<SpriteComponent>();
                const auto& cursor = entity.getComponent<CursorComponent>();

                SDL_Texture* texture = asset_manager->getTexture(sprite.textureId);

                SDL_Rect dst = {
                    mouse_x - sprite.width / 2, // center offset
                    mouse_y - sprite.height / 2,
                    static_cast<int>(sprite.width * cursor.scale),
                    static_cast<int>(sprite.height * cursor.scale)
                };

                SDL_RenderCopy(renderer, texture, &sprite.srcRect, &dst);
            }
        }
};

#endif // CURSOR_SYSTEM_HPP