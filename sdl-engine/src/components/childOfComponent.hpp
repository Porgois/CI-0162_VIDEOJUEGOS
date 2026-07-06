#ifndef CHILD_OF_COMPONENT_HPP
#define CHILD_OF_COMPONENT_HPP

#include <glm/glm.hpp>
#include "../e.c.s./ecs.hpp"

struct ChildOfComponent {
    Entity parent;
    glm::vec2 offset;
    bool has_explicit_offset;
    
    ChildOfComponent() : parent(-1), offset(glm::vec2(0.0f, 0.0f)), has_explicit_offset(false) {}
    ChildOfComponent(Entity parent, glm::vec2 offset = glm::vec2(0.0f, 0.0f), bool has_explicit_offset = false)
    : parent(parent), offset(offset), has_explicit_offset(has_explicit_offset) {}
};

#endif // CHILD_OF_COMPONENT_HPP