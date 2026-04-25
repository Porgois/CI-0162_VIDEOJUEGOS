#ifndef MOUSE_FOLLOW_COMPONENT_HPP
#define MOUSE_FOLLOW_COMPONENT_HPP

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

struct MouseFollowComponent {
    SDL_Point origin_pivot;

    MouseFollowComponent() : origin_pivot({0, 0}) {}
    MouseFollowComponent(SDL_Point origin_pivot)
        : origin_pivot(origin_pivot) {}
};

#endif // MOUSE_FOLLOW_COMPONENT_HPP