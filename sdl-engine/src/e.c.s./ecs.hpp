#ifndef ECS_HPP
#define ECS_HPP

#include <algorithm>
#include <cstddef>
#include <bitset>
#include <deque>
#include <iostream>
#include <memory>
#include <set>
#include <typeindex>
#include <vector>
#include <unordered_map>

#include "../utils/pool.hpp"

const unsigned int MAX_COMPONENTS = 64;

// Signature
typedef std::bitset<MAX_COMPONENTS> signature;

struct IComponent {
    protected:
        static int nextId;
};

template<typename TComponent>
class Component : public IComponent {
    public:
        static int getId() {
            static int id = nextId++;
            return id;
        }
};

class Entity {
    private:
        int id;
        std::string name;
    
    public:
        Entity(int id) : id(id) {}
        int getId() const;
        std::string getName() const;
        void kill();


        // Operator overrides
        bool operator==(const Entity other) const { return id == other.id; }
        bool operator!=(const Entity other) const { return id != other.id; }
        bool operator>(const Entity other) const { return id > other.id; }
        bool operator<(const Entity other) const { return id < other.id; }

        // Easy component addition
        template <typename TComponent, typename... TArgs>
        void addComponent(TArgs&&... args);

        template <typename TComponent> 
        void removeComponent();

        template <typename TComponent>
        bool hasComponent() const;

        template <typename TComponent>
        TComponent& getComponent() const;

        class Registry* registry;
};

class System {
    private:
        signature componentSignature;
        std::vector<Entity> entities;
    
    public:
    System() = default;
    ~System() = default;

    void addSystemEntity(Entity entity);
    void removeSystemEntity(Entity entity);
    std::vector<Entity> getSystemEntities() const;
    const signature& getComponentSignature() const;

    template<typename TComponent>
    void requireComponent();
};

class Registry {
    private:
        int numEntity = 0;
    
        std::vector<std::shared_ptr<IPool>> componentPools;
        std::vector<signature> entityComponentSignatures;
        
        std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

        std::set<Entity> entitiesToAdd;
        std::set<Entity> entitiesToDelete;

        std::deque<int> freeIds;

    public:
        Registry();
        ~Registry();

    void update();
    
    // Entity Management
    Entity createEntity();
    void destroyEntity(Entity entity);

    // Component Management
    template <typename TComponent, typename... TArgs>
    void addComponent(Entity entity, TArgs&&... args);

    template <typename TComponent> 
    void removeComponent(Entity entity);

    template <typename TComponent>
    bool hasComponent(Entity entity) const;

    template <typename TComponent>
    TComponent& getComponent(Entity entity) const;

    // System Management
    template <typename TSystem, typename... TArgs>
    void addSystem(TArgs&&... args);

    template <typename TSystem> 
    void removeSystem();

    template <typename TSystem>
    bool hasSystem() const;

    template <typename TSystem>
    TSystem& getSystem() const;

    // Add or remove entities from systems
    void addSystemsEntity(Entity entity);
    void removeSystemsEntity(Entity entity);

    // Reset registry
    void clearAllEntities();
};

template <typename TComponent>
void System::requireComponent() {
    const int componentId = Component<TComponent>::getId();
    componentSignature.set(componentId);
}

// Registry
template <typename TComponent, typename... TArgs>
void Registry::addComponent(Entity entity, TArgs&&... args) {
    const int componentId = Component<TComponent>::getId();
    const int entityId = entity.getId();

    if (static_cast<long unsigned int>(componentId) >= componentPools.size()) {
        componentPools.resize(componentId + 10, nullptr); // Increase size by 10, initialized as "nullptr"
    }

    if (!componentPools[componentId]) {
        std::shared_ptr<Pool<TComponent>> newComponentPool \
            = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool \
        = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    // Component pool resize if need be
    if (entityId >= componentPool->getSize()) {
        componentPool->resize(numEntity + 100); // Increase size by 100
    }

    TComponent newComponent(std::forward<TArgs>(args)...);

    componentPool->set(entityId, newComponent);
    entityComponentSignatures[entityId].set(componentId);
}

template <typename TComponent>
void Registry::removeComponent(Entity entity) {
    const int componentId = Component<TComponent>::getId();
    const int entityId = entity.getId();

    entityComponentSignatures[entityId].set(componentId, false);
}

template <typename TComponent>
bool Registry::hasComponent(Entity entity) const {
    const int componentId = Component<TComponent>::getId();
    const int entityId = entity.getId();

    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Registry::getComponent(Entity entity) const {
    const int componentId = Component<TComponent>::getId();
    const int entityId = entity.getId();

    auto componentPool \
        = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    
    return componentPool->get(entityId);
}

// System
template <typename TSystem, typename... TArgs>
void Registry::addSystem(TArgs&&... args) {
    std::shared_ptr<TSystem> new_system \
        = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), new_system));
}

template <typename TSystem>
void Registry::removeSystem() {
    auto system = systems.find(std::type_index(typeid(TSystem)));

    systems.erase(system);
}

template <typename TSystem>
bool Registry::hasSystem() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::getSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));

    return *(std::static_pointer_cast<TSystem>(system->second));
}

// Entity Add component
template <typename TComponent, typename... TArgs>
void Entity::addComponent(TArgs&&... args) {
    registry->addComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

// Entity remove component
template <typename TComponent>
void Entity::removeComponent() {
    registry->removeComponent<TComponent>(*this);
}

// Entity has component
template <typename TComponent>
bool Entity::hasComponent() const {
    return registry->hasComponent<TComponent>(*this);
}

// Entity get component
template <typename TComponent>
TComponent& Entity::getComponent() const {
    return registry->getComponent<TComponent>(*this);
}

#endif // ECS_HPP