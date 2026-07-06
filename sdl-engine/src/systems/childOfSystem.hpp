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

                glm::vec2 parent_center = glm::vec2(
                    parent_transform.position.x + parent_sprite.width  * 0.5f,
                    parent_transform.position.y + parent_sprite.height * 0.5f
                );

                // Preserve any already-set child offset on first update.
                if (!child_of.has_explicit_offset && transform.position != parent_center) {
                    child_of.offset = transform.position - parent_center;
                    child_of.has_explicit_offset = true;
                }

                transform.position = parent_center + child_of.offset;
            }
        }
};

#endif // CHILD_OF_SYSTEM