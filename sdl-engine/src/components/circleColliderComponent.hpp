#ifndef CIRCLE_COLLIDER_COMPONENT_HPP
#define CIRCLE_COLLIDER_COMPONENT_HPP

struct CircleColliderComponent {
    int radius;
    int width;
    int height;

    CircleColliderComponent(int radius = 0, int width = 0, int height = 0) {
        this->radius = radius;
        this->width = width;
        this->height = height;
    }

    glm::vec2 getCenter(TransformComponent& transform) {
        return glm::vec2(
            transform.position.x + (this->width / 2) * transform.scale.x,
            transform.position.y + (this->height / 2) * transform.scale.y
        );
    }

    float getScaledRadius(TransformComponent& transform) {
        return this->radius * transform.scale.x;
    }
};

#endif // CIRCLE_COLLIDER_COMPONENT_HPP