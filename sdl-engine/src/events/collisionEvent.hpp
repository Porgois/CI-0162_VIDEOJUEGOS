#ifndef COLLISION_EVENT_HPP
#define COLLISION_EVENT_HPP

#include "../e.c.s./ecs.hpp"
#include "../eventManager/event.hpp"


class CollisionEvent : public Event {
    public:
        Entity collider;
        Entity collidee;
    
        CollisionEvent(Entity collider, Entity collidee) : collider(collider), collidee(collidee) {}
};

#endif // COLLISION_EVENT_HPP