#ifndef BOX_COLLIDER_COMPONENT_HPP
#define BOX_COLLIDER_COMPONENT_HPP

#include <glm/glm.hpp>

#include "../components/transformComponent.hpp"

struct BoxColliderComponent {
    int width;
    int height;
    glm::vec2 offset;
    bool is_trigger;

    BoxColliderComponent(int width = 0, int height = 0, glm::vec2 offset = glm::vec2(0.0, 0.0), bool is_trigger = false) {
        this->width = width;
        this->height = height;
        this->offset = offset;
        this->is_trigger = is_trigger;
    }
};

#endif // BOX_COLLIDER_COMPONENT_HPP