#ifndef CIRCLE_COLLISION_SYSTEM_HPP
#define CIRCLE_COLLISION_SYSTEM_HPP

#include "../e.c.s./ecs.hpp"
#include "../eventManager/eventManager.hpp"
#include "../components/transformComponent.hpp"
#include "../components/circleColliderComponent.hpp"
#include "../events/collisionEvent.hpp"

class CircleCollisionSystem : public System {
    public:
        CircleCollisionSystem() {
            requireComponent<TransformComponent>();
            requireComponent<CircleColliderComponent>();
        }

        void update(std::unique_ptr<EventManager>& event_manager) {
            auto entities = getSystemEntities();

            for (auto i = entities.begin(); i != entities.end(); i++) {
                Entity a = *i;
                auto& a_collider = a.getComponent<CircleColliderComponent>();
                auto& a_transform = a.getComponent<TransformComponent>();

                for (auto j = i; j != entities.end(); j++) {
                    Entity b = *j;
                   
                    if (a == b) {
                        continue;
                    }

                    auto& b_collider = b.getComponent<CircleColliderComponent>();
                    auto& b_transform = b.getComponent<TransformComponent>();

                    bool collision = checkCircularCollision(
                        a_collider.getScaledRadius(a_transform), 
                        b_collider.getScaledRadius(b_transform),
                        a_collider.getCenter(a_transform),
                        b_collider.getCenter(b_transform)
                    );

                    if (collision) {
                        event_manager->emitEvent<CollisionEvent>(a, b);
                        std::cout << "Collision between (" << a.getName() << ") & (" << b.getName() << ")." << std::endl;
                    }
                }
            }
        }

        bool checkCircularCollision(float a_radius, float b_radius, glm::vec2 a_pos, glm::vec2 b_pos) {
            glm::vec2 dif = a_pos - b_pos;
            double length = glm::sqrt((dif.x * dif.x) + (dif.y * dif.y));
            return (a_radius + b_radius >= length);
        }
};

#endif // CIRCLE_COLLISION_SYSTEM_HPP