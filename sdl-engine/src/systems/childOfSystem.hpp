#ifndef CHILD_OF_SYSTEM_HPP
#define CHILD_OF_SYSTEM_HPP

#include <SDL2/SDL.h>

#include "../e.c.s./ecs.hpp"
#include "../components/childOfComponent.hpp"
#include "../components/transformComponent.hpp"

class ChildOfSystem : public System {
    public:
        ChildOfSystem() {
            requireComponent<TransformComponent>();
            requireComponent<ChildOfComponent>();
        }

        void update() {
            for (auto entity : getSystemEntities()) {
                auto& child_of = entity.getComponent<ChildOfComponent>();
                auto& transform = entity.getComponent<TransformComponent>();

                // Parent is valid
                if (!child_of.parent.hasComponent<TransformComponent>()) {
                    continue;
                }

                auto& parent_transform = child_of.parent.getComponent<TransformComponent>();

                // Set position
                transform.position.x = parent_transform.position.x \
                    + child_of.offset.x;
                transform.position.y = parent_transform.position.y \
                    + child_of.offset.y;
            }
        }
};

#endif // CHILD_OF_SYSTEM