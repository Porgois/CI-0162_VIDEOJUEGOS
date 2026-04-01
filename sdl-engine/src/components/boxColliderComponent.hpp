#ifndef BOX_COLLIDER_COMPONENT_HPP
#define BOX_COLLIDER_COMPONENT_HPP

#include <glm/glm.hpp>

#include "../components/transformComponent.hpp"

struct BoxColliderComponent {
    int width;
    int height;
    glm::vec2 offset; // optional collider shift relative to transform position

    BoxColliderComponent(int width = 0, int height = 0, glm::vec2 offset = glm::vec2(0.0, 0.0)) {
        this->width = width;
        this->height = height;
        this->offset = offset;
    }

    glm::vec2 getMin(const TransformComponent& transform) const {
        return glm::vec2(transform.position.x + offset.x * transform.scale.x, \
            transform.position.y + offset.y * transform.scale.y);
    }

    glm::vec2 getMax(const TransformComponent& transform) const {
        return glm::vec2(transform.position.x + (offset.x + width) * transform.scale.x, \
            transform.position.y + (offset.y + height) * transform.scale.y); 
    }
};

#endif // BOX_COLLIDER_COMPONENT_HPP