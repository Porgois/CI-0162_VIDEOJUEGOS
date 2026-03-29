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

    glm::vec2 getCenter(TransformComponent& t) {
        return glm::vec2(
            t.position.x + (this->width  / 2.0f) * t.scale.x,
            t.position.y + (this->height / 2.0f) * t.scale.y
        );
    }

    float getScaledRadius(TransformComponent& t) {
        return this->radius * t.scale.x;
    }
};

#endif // CIRCLE_COLLIDER_COMPONENT_HPP