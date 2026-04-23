#ifndef RIGIDBODY_COMPONENT_HPP
#define RIGIDBODY_COMPONENT_HPP

#include <glm/glm.hpp>

struct RigidBodyComponent {
    bool is_dynamic;
    bool is_solid;
    
    glm::vec2 sum_forces = glm::vec2(0);
    glm::vec2 acceleration = glm::vec2(0);
    glm::vec2 velocity = glm::vec2(0);

    float mass;
    float inverse_mass;

    RigidBodyComponent(bool is_dynamic = false, bool is_solid = false, float mass = 1.0f) {
        this->is_dynamic = is_dynamic;
        this->is_solid = is_solid;
        this->mass = mass;
        this->inverse_mass = 1 / mass;
    }
};

#endif // RIGIDBODY_COMPONENT_HPP