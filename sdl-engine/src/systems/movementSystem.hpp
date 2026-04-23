#ifndef MOVEMENT_SYSTEM_HPP
#define MOVEMENT_SYSTEM_HPP

#include <glm/glm.hpp>

#include "../components/rigidBodyComponent.hpp"
#include "../components/transformComponent.hpp"
#include "../e.c.s./ecs.hpp"


class MovementSystem : public System {
    public:
        MovementSystem() {
            requireComponent<RigidBodyComponent>();
            requireComponent<TransformComponent>();
        }
    
        void update(double delta_time) {
            for (auto entity : getSystemEntities()) {
                // Get neccesary components
                auto& transform = entity.getComponent<TransformComponent>();
                auto& rigid_body = entity.getComponent<RigidBodyComponent>();

                // Update previous position
                transform.previous_position = transform.position;

                // Diferentiate between dynamic and static bodies for behavior
                if (rigid_body.is_dynamic) {
                    rigid_body.acceleration = rigid_body.sum_forces * rigid_body.inverse_mass;
                    rigid_body.velocity += rigid_body.acceleration * static_cast<float>(delta_time);
                    transform.position += rigid_body.velocity * static_cast<float>(delta_time);
                    rigid_body.sum_forces = glm::vec2(0); // reset forces

                } else { // static
                    // Update velocity
                    transform.position.x += rigid_body.velocity.x * delta_time;
                    transform.position.y += rigid_body.velocity.y * delta_time;
                }
            }
        };
};

#endif // MOVEMENT_SYSTEM_HPP
