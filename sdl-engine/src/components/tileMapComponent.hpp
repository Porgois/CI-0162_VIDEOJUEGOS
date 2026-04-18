#ifndef TILE_MAP_COMPONENT_HPP
#define TILE_MAP_COMPONENT_HPP

#include <SDL2/SDL.h>

struct TileMapComponent {
    SDL_Texture* baked_texture = nullptr;
    // Layer pixel dimensions
    int width = 0;
    int height = 0;

    TileMapComponent() = default;

    TileMapComponent(SDL_Texture* texture, int w, int h) \
        : baked_texture(texture), width(w), height(h) {}
    
    TileMapComponent(const TileMapComponent&) = delete;
    TileMapComponent& operator=(const TileMapComponent&) = delete;
    TileMapComponent(TileMapComponent&&) = default;
    TileMapComponent& operator=(TileMapComponent&&) = default;
};

#endif // TILE_MAP_COMPONENT_HPP