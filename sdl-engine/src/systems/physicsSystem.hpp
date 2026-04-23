#ifndef PHYSICS_SYSTEM_HPP
#define PHYSICS_SYSTEM_HPP


#include "../e.c.s./ecs.hpp"
#include "../components/rigidBodyComponent.hpp"

class PhysicsSystem : public System {
    private:
        float gravity = 9.8f;
        int meter_conversion_unit = 64;

    public:
        PhysicsSystem() {
            requireComponent<RigidBodyComponent>();
        }

        void update() {
            for (auto entity : getSystemEntities()) {
                auto& rigid_body = entity.getComponent<RigidBodyComponent>();

                // Apply gravity
                if (rigid_body.is_dynamic) {
                    rigid_body.sum_forces += glm::vec2(0.0f, (gravity * rigid_body.mass * meter_conversion_unit));
                }
            }
        }
};

#endif // PHYSICS_SYSTEM_HPP