#ifndef OVERLAP_SYSTEM_HPP
#define OVERLAP_SYSTEM_HPP

#include "../components/rigidBodyComponent.hpp"
#include "../components/boxColliderComponent.hpp"
#include "../components/transformComponent.hpp"
#include "../eventManager/eventManager.hpp"
#include "../events/collisionEvent.hpp"
#include "../e.c.s./ecs.hpp"

enum Direction {top, left, bottom, right};

class OverlapSystem : public System {
    private:

        bool checkCollision(Entity a, Entity b, Direction dir) {
            // A
            auto& a_collider = a.getComponent<BoxColliderComponent>();
            auto& a_transform = a.getComponent<TransformComponent>();
            float a_x = a_transform.previous_position.x + a_collider.offset.x;
            float a_y = a_transform.previous_position.y + a_collider.offset.y;
            float a_w = static_cast<float>(a_collider.width);
            float a_h = static_cast<float>(a_collider.height);

            // B
            auto& b_collider = b.getComponent<BoxColliderComponent>();
            auto& b_transform = b.getComponent<TransformComponent>();
            float b_x = b_transform.previous_position.x + b_collider.offset.x;
            float b_y = b_transform.previous_position.y + b_collider.offset.y;
            float b_w = static_cast<float>(b_collider.width);
            float b_h = static_cast<float>(b_collider.height);

            // A's top side against b's lower side
            if (Direction::top == dir) {
                return (
                    a_x < b_x + b_w &&
                    a_x + a_w > b_x &&
                    a_y > b_y
                );
            }

            // A's bottom side against b's upper side
            if (Direction::bottom == dir) {
                return (
                    a_x < b_x + b_w &&
                    a_x + a_w > b_x &&
                    a_y < b_y
                );
            }

            // A's left side against b's right side
            if (Direction::left == dir) {
                return (
                    a_y < b_y + b_h &&
                    a_y + a_h > b_y &&
                    a_x > b_x
                );
            }

            // A's right side against b's left side
            if (Direction::right == dir) {
                return (
                    a_y < b_y + b_h &&
                    a_y + a_h > b_y &&
                    a_x < b_x
                );
            }

            // No collision detected
            return false;
        }

        void avoidOverlap(Entity a, Entity b) {
            auto& a_collider = a.getComponent<BoxColliderComponent>();
            auto& a_transform = a.getComponent<TransformComponent>();

            auto& b_collider = b.getComponent<BoxColliderComponent>();
            auto& b_transform = b.getComponent<TransformComponent>();
            auto& b_rigidbody = b.getComponent<RigidBodyComponent>();

            if (checkCollision(a, b, Direction::top)) { // TOP
                b_transform.position = glm::vec2(b_transform.position.x, (a_transform.position.y + a_collider.offset.y) - \
                    (b_collider.height + b_collider.offset.y));
                b_rigidbody.velocity = glm::vec2(b_rigidbody.velocity.x, 0.0f);
            } else if (checkCollision(a, b, Direction::bottom)) { // BOTTOM
                b_transform.position = glm::vec2(b_transform.position.x, (a_transform.position.y + a_collider.offset.y) + \
                    a_collider.height - b_collider.offset.y);
                b_rigidbody.velocity = glm::vec2(b_rigidbody.velocity.x, 0.0f);
            } else if (checkCollision(a, b, Direction::left)) { // LEFT
                b_transform.position = glm::vec2((a_transform.position.x + a_collider.offset.x) - \
                    (b_collider.width + b_collider.offset.x), b_transform.position.y);
                b_rigidbody.velocity = glm::vec2(0.0f,  b_rigidbody.velocity.y);
            } else if (checkCollision(a, b, Direction::right)) { // RIGHT
                b_transform.position = glm::vec2((a_transform.position.x + a_collider.offset.x) + 
                    a_collider.width - b_collider.offset.x, b_transform.position.y);
                b_rigidbody.velocity = glm::vec2(0.0f,  b_rigidbody.velocity.y);
            }
        }

    public:
        OverlapSystem() {
            requireComponent<RigidBodyComponent>();
            requireComponent<BoxColliderComponent>();
            requireComponent<TransformComponent>();
        }

        void subscribeCollisionEvent(const std::unique_ptr<EventManager>& event_manager) {
            event_manager->subscribeEvent<CollisionEvent, OverlapSystem>(this, &OverlapSystem::onCollisionEvent);
        }

        void onCollisionEvent(CollisionEvent& e) {
            auto& a_rigidbody = e.collider.getComponent<RigidBodyComponent>();
            auto& b_rigidbody = e.collidee.getComponent<RigidBodyComponent>();

            if (a_rigidbody.is_solid && b_rigidbody.is_solid) {
                if (a_rigidbody.mass >= b_rigidbody.mass) {
                    avoidOverlap(e.collider, e.collidee);
                } else {
                    avoidOverlap(e.collidee, e.collider);
                }
            }
        }
};

#endif // PHYSICS_SYSTEM_HPP