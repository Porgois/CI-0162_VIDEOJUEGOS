#ifndef COLLISION_SYSTEM_HPP
#define COLLISION_SYSTEM_HPP

#include <memory>

#include "../e.c.s./ecs.hpp"
#include "../eventManager/eventManager.hpp"
#include "../components/transformComponent.hpp"
#include "../components/circleColliderComponent.hpp"
#include "../components/boxColliderComponent.hpp"
#include "../components/colliderComponent.hpp"
#include "../events/collisionEvent.hpp"

class CollisionSystem : public System {
    public:
        CollisionSystem() {
            requireComponent<TransformComponent>();
            requireComponent<ColliderComponent>();
        }

        // --- Shared resolution logic ---
        void applyResolution(Entity a, Entity b, glm::vec2 normal, float penetration) {
            auto& a_transform = a.getComponent<TransformComponent>();
            auto& b_transform = b.getComponent<TransformComponent>();

            bool a_dynamic = a.hasComponent<RigidBodyComponent>();
            bool b_dynamic = b.hasComponent<RigidBodyComponent>();

            if (a_dynamic && b_dynamic) {
                glm::vec2 correction = normal * (penetration / 2.0f);
                a_transform.position.x += correction.x;
                a_transform.position.y += correction.y;
                b_transform.position.x -= correction.x;
                b_transform.position.y -= correction.y;
            } else if (a_dynamic) {
                a_transform.position.x += normal.x * penetration;
                a_transform.position.y += normal.y * penetration;
            } else if (b_dynamic) {
                b_transform.position.x -= normal.x * penetration;
                b_transform.position.y -= normal.y * penetration;
            }

            if (a_dynamic) {
                auto& a_rb = a.getComponent<RigidBodyComponent>();
                glm::vec2 b_vel = b_dynamic
                    ? b.getComponent<RigidBodyComponent>().velocity
                    : glm::vec2(0.0f);

                glm::vec2 rel_vel = a_rb.velocity - b_vel;
                float vel_along_normal = glm::dot(rel_vel, normal);

                if (vel_along_normal < 0) {
                    float restitution = 0.8f;
                    a_rb.velocity -= vel_along_normal * normal * restitution;
                }
            }

            if (b_dynamic) {
                auto& b_rb = b.getComponent<RigidBodyComponent>();
                glm::vec2 a_vel = a_dynamic
                    ? a.getComponent<RigidBodyComponent>().velocity
                    : glm::vec2(0.0f);

                glm::vec2 rel_vel = b_rb.velocity - a_vel;
                float vel_along_normal = glm::dot(rel_vel, -normal);

                if (vel_along_normal < 0) {
                    float restitution = 0.8f;
                    b_rb.velocity -= vel_along_normal * -normal * restitution;
                }
            }
        }

        // --- Circle vs Circle ---
        void resolveCircleCircle(Entity a, Entity b) {
            auto& a_collider = a.getComponent<CircleColliderComponent>();
            auto& a_transform = a.getComponent<TransformComponent>();
            auto& b_collider = b.getComponent<CircleColliderComponent>();
            auto& b_transform = b.getComponent<TransformComponent>();

            glm::vec2 a_pos   = a_collider.getCenter(a_transform);
            glm::vec2 b_pos   = b_collider.getCenter(b_transform);
            float a_radius    = a_collider.getScaledRadius(a_transform);
            float b_radius    = b_collider.getScaledRadius(b_transform);

            glm::vec2 normal  = a_pos - b_pos;
            float distance    = glm::length(normal);

            if (distance == 0.0f) normal = glm::vec2(1, 0);
            else normal = glm::normalize(normal);

            float penetration = (a_radius + b_radius) - distance;
            applyResolution(a, b, normal, penetration);
        }

        // --- Rect vs Rect ---
        void resolveBoxRect(Entity a, Entity b) {
            auto& a_collider  = a.getComponent<BoxColliderComponent>();
            auto& a_transform = a.getComponent<TransformComponent>();
            auto& b_collider  = b.getComponent<BoxColliderComponent>();
            auto& b_transform = b.getComponent<TransformComponent>();

            glm::vec2 a_min = a_collider.getMin(a_transform);
            glm::vec2 a_max = a_collider.getMax(a_transform);
            glm::vec2 b_min = b_collider.getMin(b_transform);
            glm::vec2 b_max = b_collider.getMax(b_transform);

            float overlap_x = std::min(a_max.x, b_max.x) - std::max(a_min.x, b_min.x);
            float overlap_y = std::min(a_max.y, b_max.y) - std::max(a_min.y, b_min.y);

            glm::vec2 normal;
            float penetration;

            if (overlap_x < overlap_y) {
                penetration = overlap_x;
                normal = (a_min.x < b_min.x) ? glm::vec2(-1, 0) : glm::vec2(1, 0);
            } else {
                penetration = overlap_y;
                normal = (a_min.y < b_min.y) ? glm::vec2(0, -1) : glm::vec2(0, 1);
            }

            applyResolution(a, b, normal, penetration);
        }

        // --- Circle vs Rect (circle = a, rect = b) ---
        void resolveCircleRect(Entity circle_entity, Entity rect_entity) {
            auto& c_collider  = circle_entity.getComponent<CircleColliderComponent>();
            auto& c_transform = circle_entity.getComponent<TransformComponent>();
            auto& r_collider  = rect_entity.getComponent<BoxColliderComponent>();
            auto& r_transform = rect_entity.getComponent<TransformComponent>();

            glm::vec2 center = c_collider.getCenter(c_transform);
            float radius     = c_collider.getScaledRadius(c_transform);
            glm::vec2 r_min  = r_collider.getMin(r_transform);
            glm::vec2 r_max  = r_collider.getMax(r_transform);

            glm::vec2 closest = glm::vec2(
                std::clamp(center.x, r_min.x, r_max.x),
                std::clamp(center.y, r_min.y, r_max.y)
            );

            glm::vec2 diff  = center - closest;
            float distance  = glm::length(diff);

            glm::vec2 normal;
            float penetration;

            if (distance == 0.0f) {
                float overlap_x = std::min(center.x - r_min.x, r_max.x - center.x);
                float overlap_y = std::min(center.y - r_min.y, r_max.y - center.y);

                if (overlap_x < overlap_y) {
                    normal      = (center.x < (r_min.x + r_max.x) / 2) ? glm::vec2(-1, 0) : glm::vec2(1, 0);
                    penetration = overlap_x + radius;
                } else {
                    normal      = (center.y < (r_min.y + r_max.y) / 2) ? glm::vec2(0, -1) : glm::vec2(0, 1);
                    penetration = overlap_y + radius;
                }
            } else {
                normal      = glm::normalize(diff);
                penetration = radius - distance;
            }

            applyResolution(circle_entity, rect_entity, normal, penetration);
        }

        void update(std::unique_ptr<EventManager>& event_manager) {
            auto entities = getSystemEntities();

            for (auto i = entities.begin(); i != entities.end(); i++) {
                Entity a = *i;
                for (auto j = i; j != entities.end(); j++) {
                    Entity b = *j;
                    if (a == b) continue;

                    bool a_circle = a.hasComponent<CircleColliderComponent>();
                    bool b_circle = b.hasComponent<CircleColliderComponent>();
                    bool a_rect   = a.hasComponent<BoxColliderComponent>();
                    bool b_rect   = b.hasComponent<BoxColliderComponent>();

                    bool collision = false;

                    if (a_circle && b_circle) {
                        auto& ac = a.getComponent<CircleColliderComponent>();
                        auto& at = a.getComponent<TransformComponent>();
                        auto& bc = b.getComponent<CircleColliderComponent>();
                        auto& bt = b.getComponent<TransformComponent>();
                        collision = checkCircularCollision(
                            ac.getScaledRadius(at), bc.getScaledRadius(bt),
                            ac.getCenter(at),       bc.getCenter(bt)
                        );
                        if (collision) resolveCircleCircle(a, b);

                    } else if (a_rect && b_rect) {
                        auto& ar = a.getComponent<BoxColliderComponent>();
                        auto& at = a.getComponent<TransformComponent>();
                        auto& br = b.getComponent<BoxColliderComponent>();
                        auto& bt = b.getComponent<TransformComponent>();
                        collision = checkBoxCollision(  // ← system method, not entity
                            ar.getMin(at), ar.getMax(at),
                            br.getMin(bt), br.getMax(bt)
                        );
                        if (collision) resolveBoxRect(a, b);

                    } else if (a_circle && b_rect) {
                        auto& ac = a.getComponent<CircleColliderComponent>();
                        auto& at = a.getComponent<TransformComponent>();
                        auto& br = b.getComponent<BoxColliderComponent>();
                        auto& bt = b.getComponent<TransformComponent>();
                        collision = checkCircleRectCollision(
                            ac.getCenter(at), ac.getScaledRadius(at),
                            br.getMin(bt),    br.getMax(bt)
                        );
                        if (collision) resolveCircleRect(a, b);

                    } else if (a_rect && b_circle) {
                        auto& ar = a.getComponent<BoxColliderComponent>();
                        auto& at = a.getComponent<TransformComponent>();
                        auto& bc = b.getComponent<CircleColliderComponent>();
                        auto& bt = b.getComponent<TransformComponent>();
                        collision = checkCircleRectCollision(
                            bc.getCenter(bt), bc.getScaledRadius(bt),
                            ar.getMin(at),    ar.getMax(at)
                        );
                        if (collision) resolveCircleRect(b, a); // b is circle, a is rect
                    }

                    if (collision) {
                        event_manager->emitEvent<CollisionEvent>(a, b);
                        std::cout << "Collision between (" << a.getName() << ") & (" << b.getName() << ")." << std::endl;
                    }
                }
            }
        }

        bool checkCircularCollision(float a_radius, float b_radius, glm::vec2 a_pos, glm::vec2 b_pos) {
            float distance = glm::length(a_pos - b_pos);
            return (a_radius + b_radius >= distance);
        }

        bool checkCircleRectCollision(glm::vec2 center, float radius, glm::vec2 r_min, glm::vec2 r_max) {
            glm::vec2 closest = glm::vec2(
                std::clamp(center.x, r_min.x, r_max.x),
                std::clamp(center.y, r_min.y, r_max.y)
            );
            return glm::length(center - closest) < radius;
        }

        bool checkBoxCollision(glm::vec2 a_min, glm::vec2 a_max, glm::vec2 b_min, glm::vec2 b_max) {
            return (a_min.x < b_max.x && a_max.x > b_min.x &&
                    a_min.y < b_max.y && a_max.y > b_min.y);
        }
};

#endif // COLLISION_SYSTEM_HPP