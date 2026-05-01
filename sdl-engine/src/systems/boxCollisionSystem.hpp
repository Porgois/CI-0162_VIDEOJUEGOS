#ifndef BOX_COLLISION_SYSTEM_HPP
#define BOX_COLLISION_SYSTEM_HPP

#include <memory>
#include <cfloat>
#include <cmath>

#include "../e.c.s./ecs.hpp"
#include "../eventManager/eventManager.hpp"
#include "../components/transformComponent.hpp"
#include "../components/boxColliderComponent.hpp"
#include "../components/scriptComponent.hpp"
#include "../events/collisionEvent.hpp"

class BoxCollisionSystem : public System {
    private:
       private:
        // Project an OBB onto an axis and return [min, max]
        glm::vec2 projectOBB(glm::vec2 center, float half_w, float half_h, float rotation, glm::vec2 axis) {
            float rad = glm::radians(rotation);
            // The 4 corners relative to center
            glm::vec2 corners[4] = {
                { -half_w, -half_h },
                {  half_w, -half_h },
                {  half_w,  half_h },
                { -half_w,  half_h }
            };
            float cosR = std::cos(rad);
            float sinR = std::sin(rad);
            float mn =  FLT_MAX;
            float mx = -FLT_MAX;
            for (auto& c : corners) {
                // Corner rotation
                glm::vec2 rotated = {
                    c.x * cosR - c.y * sinR,
                    c.x * sinR + c.y * cosR
                };
                float proj = glm::dot(center + rotated, axis);
                mn = std::min(mn, proj);
                mx = std::max(mx, proj);
            }
            return { mn, mx };
        }

        bool checkOBBCollision(
            glm::vec2 a_pos, float a_w, float a_h, glm::vec2 a_offset, float a_rot,
            glm::vec2 b_pos, float b_w, float b_h, glm::vec2 b_offset, float b_rot
        ) {
            auto rotateOffset = [](glm::vec2 offset, float degrees) -> glm::vec2 {
                float rad = glm::radians(degrees);
                return {
                    offset.x * std::cos(rad) - offset.y * std::sin(rad),
                    offset.x * std::sin(rad) + offset.y * std::cos(rad)
                };
            };

            // Offset rotation
            glm::vec2 a_center = a_pos + rotateOffset(a_offset, a_rot) + glm::vec2(a_w * 0.5f, a_h * 0.5f);
            glm::vec2 b_center = b_pos + rotateOffset(b_offset, b_rot) + glm::vec2(b_w * 0.5f, b_h * 0.5f);

            float a_rad = glm::radians(a_rot);
            float b_rad = glm::radians(b_rot);

            glm::vec2 axes[4] = {
                {  std::cos(a_rad), std::sin(a_rad) },
                { -std::sin(a_rad), std::cos(a_rad) },
                {  std::cos(b_rad), std::sin(b_rad) },
                { -std::sin(b_rad), std::cos(b_rad) }
            };

            for (auto& axis : axes) {
                glm::vec2 a_proj = projectOBB(a_center, a_w * 0.5f, a_h * 0.5f, a_rot, axis);
                glm::vec2 b_proj = projectOBB(b_center, b_w * 0.5f, b_h * 0.5f, b_rot, axis);

                if (a_proj.y < b_proj.x || b_proj.y < a_proj.x) {
                    return false;
                } 
            }
            return true;
        }

    public:
        BoxCollisionSystem() {
            requireComponent<TransformComponent>();
            requireComponent<BoxColliderComponent>();
        }

        void update(const std::unique_ptr<EventManager>& event_manager, \
            sol::state& lua) {
            auto entities = getSystemEntities();

            for (auto i = entities.begin(); i != entities.end(); i++) {
                Entity a = *i;
                const auto& a_collider = a.getComponent<BoxColliderComponent>();
                const auto& a_transform = a.getComponent<TransformComponent>();

                for (auto j = i + 1; j != entities.end(); j++) {
                    Entity b = *j;
                   
                    const auto& b_collider = b.getComponent<BoxColliderComponent>();
                    const auto& b_transform = b.getComponent<TransformComponent>();

                   bool collision = checkOBBCollision(
                        a_transform.position,
                        static_cast<float>(a_collider.width  * a_transform.scale.x),
                        static_cast<float>(a_collider.height * a_transform.scale.y),
                        a_collider.offset * a_transform.scale,
                        a_transform.rotation,

                        b_transform.position,
                        static_cast<float>(b_collider.width  * b_transform.scale.x),
                        static_cast<float>(b_collider.height * b_transform.scale.y),
                        b_collider.offset * b_transform.scale,
                        b_transform.rotation
                    );

                    if (collision) {
                        event_manager->emitEvent<CollisionEvent>(a, b);

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