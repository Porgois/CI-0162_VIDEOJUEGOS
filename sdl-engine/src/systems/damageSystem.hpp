#ifndef DAMAGE_SYSTEM_HPP
#define DAMAGE_SYSTEM_HPP

#include <iostream> // TODO: delete later
#include <memory>

#include "../e.c.s./ecs.hpp"
#include "../eventManager/eventManager.hpp"
#include "../components/circleColliderComponent.hpp"
#include "../events/collisionEvent.hpp"

class DamageSystem : public System {
    public:
        DamageSystem() {
            requireComponent<CircleColliderComponent>();
        }

        void subscribeCollisionEvent(std::unique_ptr<EventManager>& event_manager) {
            event_manager->subscribeEvent<CollisionEvent, DamageSystem>(this, &DamageSystem::onCollision);
        }

        void onCollision(CollisionEvent& e) {
            std::cout << "[DAMAGE SYSTEM] Collision between " << e.collider.getId() << \
                " and " << e.collidee.getId() << std::endl;
            e.collider.kill();
            e.collidee.kill();
        }
};

#endif // DAMAGE_SYSTEM_HPP