#ifndef BOX_COLLISION_SYSTEM_HPP
#define BOX_COLLISION_SYSTEM_HPP

#include <iostream>

#include "../e.c.s./ecs.hpp"
#include "../eventManager/eventManager.hpp"
#include "../components/transformComponent.hpp"
#include "../components/boxColliderComponent.hpp"
#include "../components/scriptComponent.hpp"
#include "../events/collisionEvent.hpp"

class BoxCollisionSystem : public System {
    private:
        bool checkAABBCollision(float a_x, float a_y, float a_w, float a_h, \
            float b_x, float b_y, float b_w, float b_h) {
            return (
                a_x < b_x + b_w &&
                a_x + a_w > b_x &&
                a_y < b_y + b_h &&
                a_y + a_h > b_y
            );
        }

    public:
        BoxCollisionSystem() {
            requireComponent<TransformComponent>();
            requireComponent<BoxColliderComponent>();
        }

        void update(sol::state& lua) {
            auto entities = getSystemEntities();

            for (auto i = entities.begin(); i != entities.end(); i++) {
                Entity a = *i;
                const auto& a_collider = a.getComponent<BoxColliderComponent>();
                const auto& a_transform = a.getComponent<TransformComponent>();

                for (auto j = i + 1; j != entities.end(); j++) {
                    Entity b = *j;
                   
                    const auto& b_collider = b.getComponent<BoxColliderComponent>();
                    const auto& b_transform = b.getComponent<TransformComponent>();

                    bool collision = checkAABBCollision( // take scale into account for offset
                        a_transform.position.x + (a_collider.offset.x * a_transform.scale.x),
                        a_transform.position.y + (a_collider.offset.y * a_transform.scale.y),
                        static_cast<float>(a_collider.width * a_transform.scale.x), 
                        static_cast<float>(a_collider.height * a_transform.scale.y),
                        b_transform.position.x + (b_collider.offset.x * b_transform.scale.x), 
                        b_transform.position.y + (b_collider.offset.y * b_transform.scale.y),
                        static_cast<float>(b_collider.width * b_transform.scale.x),
                        static_cast<float>(b_collider.height * b_transform.scale.y)
                    );

                    if (collision) {
                        // Entity a
                        if (a.hasComponent<ScriptComponent>()) {
                            const auto& script = a.getComponent<ScriptComponent>();

                            if (script.onCollision != sol::nil) {
                                lua["this"] = a;
                                script.onCollision(b);
                            }
                        }
                        // Entity b
                        if (b.hasComponent<ScriptComponent>()) {
                            const auto& script = b.getComponent<ScriptComponent>();

                            if (script.onCollision != sol::nil) {
                                lua["this"] = b;
                                script.onCollision(a);
                            }
                        }
                    }                            
                }
            }
           
        }
};

#endif // BOX_COLLISION_SYSTEM_HPP