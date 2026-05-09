#ifndef MOUSE_FOLLOW_COMPONENT_HPP
#define MOUSE_FOLLOW_COMPONENT_HPP

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

struct MouseFollowComponent {
    float orbit_radius;
    bool is_active = false;
    float last_angle = 0.0f;
    MouseFollowComponent() : orbit_radius(0.0f) {}
    MouseFollowComponent(float orbit_radius) : orbit_radius(orbit_radius) {}
};

#endif // MOUSE_FOLLOW_COMPONENT_HPP