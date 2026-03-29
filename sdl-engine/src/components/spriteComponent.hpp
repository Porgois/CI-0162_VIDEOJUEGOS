#ifndef SPRITE_COMPONENT_HPP
#define SPRITE_COMPONENT_HPP

#include <SDL2/SDL.h>
#include <string>

struct SpriteComponent {
    std::string textureId;
    int width;
    int height;
    SDL_Rect srcRect;
    
    // Sprite flipping
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    bool flip_to_mouse = true;

    SpriteComponent(
        const std::string& textureId = "none", int width = 0, int height = 0, int srcRectX = 0, int srcRectY = 0, bool flip_to_mouse = false) {
        this->textureId = textureId;
        this->width = width;
        this->height = height;
        this->srcRect = {srcRectX, srcRectY, width, height};
        this->flip_to_mouse = flip_to_mouse;
    }
};

#endif // SPRITE_COMPONENT_HPP