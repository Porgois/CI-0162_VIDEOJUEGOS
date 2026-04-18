#ifndef MOVEMENT_SYSTEM_HPP
#define MOVEMENT_SYSTEM_HPP

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
                const auto& rigid_body = entity.getComponent<RigidBodyComponent>();

                // Update previous position
                transform.previous_position = transform.position;
                
                // Update velocity
                transform.position.x += rigid_body.velocity.x * delta_time;
                transform.position.y += rigid_body.velocity.y * delta_time;
            }
        };
};

#endif // MOVEMENT_SYSTEM_HPP
