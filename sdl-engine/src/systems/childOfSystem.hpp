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

                if (!child_of.parent.hasComponent<TransformComponent>()) {
                    continue;
                }

                auto& parent_transform = child_of.parent.getComponent<TransformComponent>();
                auto& parent_sprite = child_of.parent.getComponent<SpriteComponent>();

                // Child at center of parent position
                transform.position.x = parent_transform.position.x + parent_sprite.width / 2;
                transform.position.y = parent_transform.position.y + parent_sprite.height / 2;
            }
        }
};

#endif // CHILD_OF_SYSTEM