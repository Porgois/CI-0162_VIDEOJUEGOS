#ifndef CHILD_OF_COMPONENT_HPP
#define CHILD_OF_COMPONENT_HPP

#include <glm/glm.hpp>
#include "../e.c.s./ecs.hpp"

struct ChildOfComponent {
    Entity parent;
    glm::vec2 offset;
    
    ChildOfComponent() : parent(-1) {}
    ChildOfComponent(Entity parent, glm::vec2 offset = glm::vec2(0.0, 0.0))
    : parent(parent), offset(offset) {}
};

#endif // CHILD_OF_COMPONENT_HPP