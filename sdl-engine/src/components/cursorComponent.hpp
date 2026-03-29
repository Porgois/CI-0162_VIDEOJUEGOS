#ifndef CURSOR_COMPONENT_HPP
#define CURSOR_COMPONENT_HPP

#include "../components/spriteComponent.hpp"
#include <SDL2/SDL.h>
#include <string>

// Works as a tag
struct CursorComponent {
    int scale; // single scale value for x and y
    CursorComponent(int scale = 4) {
        this->scale = scale;
    };
};

#endif // SPRITE_COMPONENT_HPP