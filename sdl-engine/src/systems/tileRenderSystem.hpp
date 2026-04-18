#ifndef TILE_RENDER_SYSTEM_HPP
#define TILE_RENDER_SYSTEM_HPP

#include <SDL2/SDL.h>
#include "../e.c.s./ecs.hpp"
#include "../components/tileMapComponent.hpp"
#include "../components/layerComponent.hpp"

class TileRenderSystem : public System {
public:
    TileRenderSystem() {
        requireComponent<TileMapComponent>();
        requireComponent<LayerComponent>();
    }

    void update(SDL_Renderer* renderer, SDL_Rect& camera, float zoom_level) {
        auto layers = getSystemEntities();
       
        std::sort(layers.begin(), layers.end(), [](Entity a, Entity b) {
            return a.getComponent<LayerComponent>().z_index > 
                b.getComponent<LayerComponent>().z_index;
        });

        for (auto& layer : layers) {
            const auto& tile_map = layer.getComponent<TileMapComponent>();
            SDL_Rect src = { 0, 0, tile_map.width, tile_map.height };
            SDL_Rect dst = {
                static_cast<int>(-camera.x),
                static_cast<int>(-camera.y),
                static_cast<int>(tile_map.width  * zoom_level),
                static_cast<int>(tile_map.height * zoom_level)
            };
            SDL_RenderCopy(renderer, tile_map.baked_texture, &src, &dst);
        }
    }
};

#endif // TILE_RENDER_SYSTEM_HPP