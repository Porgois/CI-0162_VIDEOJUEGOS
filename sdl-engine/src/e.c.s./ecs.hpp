#ifndef ECS_HPP
#define ECS_HPP

#include <algorithm>
#include <cstddef>
#include <bitset>
#include <deque>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <stdexcept>
#include <tuple>
#include <typeindex>
#include <utility>
#include <vector>
#include <unordered_map>

#include "../utils/pool.hpp"

const unsigned int MAX_COMPONENTS = 64;

typedef std::bitset<MAX_COMPONENTS> signature;
using EntityIndex = int;
using EntityGeneration = unsigned int;

struct IComponent
{
protected:
    static int nextId;
};

template <typename TComponent>
class Component : public IComponent
{
public:
    static int getId()
    {
        static int id = nextId++;
        return id;
    }
};

class Registry;

class Entity
{
private:
    EntityIndex index;
    EntityGeneration generation;
    std::string name;

public:
    Entity(EntityIndex index = -1, EntityGeneration generation = 0)
        : index(index), generation(generation) {}

    EntityIndex getId() const;
    EntityIndex getIndex() const;
    EntityGeneration getGeneration() const;
    std::string getName() const;
    void kill();

    // Operator overrides
    bool operator==(const Entity other) const
    {
        return index == other.index && generation == other.generation;
    }
    bool operator!=(const Entity other) const
    {
        return !(*this == other);
    }
    bool operator>(const Entity other) const
    {
        return std::tie(index, generation) > std::tie(other.index, other.generation);
    }
    bool operator<(const Entity other) const
    {
        return std::tie(index, generation) < std::tie(other.index, other.generation);
    }

    // Easy component addition
    template <typename TComponent, typename... TArgs>
    void addComponent(TArgs &&...args);

    template <typename TComponent>
    void removeComponent();

    template <typename TComponent>
    bool hasComponent() const;

    template <typename TComponent>
    TComponent &getComponent() const;

    Registry *registry = nullptr;
};

class System
{
private:
    signature componentSignature;
    std::vector<Entity> entities;

public:
    System() = default;
    ~System() = default;

    void addSystemEntity(Entity entity);
    void removeSystemEntity(Entity entity);
    void clearEntities();
    std::vector<Entity> getSystemEntities() const;
    const signature &getComponentSignature() const;

    template <typename TComponent>
    void requireComponent();
};

class Registry
{
private:
    int numEntity = 0;

    std::vector<std::shared_ptr<IPool>> componentPools;
    std::vector<signature> entityComponentSignatures;
    std::vector<EntityGeneration> entityGenerations;
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

    std::set<Entity> entitiesToAdd;
    std::set<Entity> entitiesToDelete;
    std::unordered_map<std::string, Entity> namedEntities;
    std::deque<EntityIndex> freeIndices;

public:
    Registry();
    ~Registry();

    void update();

    // Entity Management
    Entity createEntity();
    void destroyEntity(Entity entity);
    bool isEntityValid(Entity entity) const;
    void unregisterEntityName(Entity entity);

    template <typename TComponent, typename... TArgs>
    void addComponent(Entity entity, TArgs &&...args);

    template <typename TComponent>
    void removeComponent(Entity entity);

    template <typename TComponent>
    bool hasComponent(Entity entity) const;

    template <typename TComponent>
    TComponent &getComponent(Entity entity) const;

    // System Management
    template <typename TSystem, typename... TArgs>
    void addSystem(TArgs &&...args);

    template <typename TSystem>
    void removeSystem();

    template <typename TSystem>
    bool hasSystem() const;

    template <typename TSystem>
    TSystem &getSystem() const;

    // Add or remove entities from systems
    void addSystemsEntity(Entity entity);
    void removeSystemsEntity(Entity entity);

    // Find entity
    void registerEntityName(const std::string &name, Entity entity);
    Entity findEntity(const std::string &name) const;
    bool hasEntity(const std::string &name) const;

    // Reset registry
    void clearAllEntities();
};

template <typename TComponent>
void System::requireComponent()
{
    const int componentId = Component<TComponent>::getId();
    componentSignature.set(componentId);
}

// Registry
template <typename TComponent, typename... TArgs>
void Registry::addComponent(Entity entity, TArgs &&...args)
{
    const int componentId = Component<TComponent>::getId();
    const int entityId = entity.getId();

    if (!isEntityValid(entity))
    {
        throw std::runtime_error("addComponent: invalid entity");
    }

    if (static_cast<size_t>(componentId) >= componentPools.size())
    {
        componentPools.resize(componentId + 10, nullptr);
    }

    if (!componentPools[componentId])
    {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    if (static_cast<size_t>(entityId) >= componentPool->getSize())
    {
        componentPool->resize(entityId + 1);
    }

    TComponent newComponent(std::forward<TArgs>(args)...);
    componentPool->set(static_cast<unsigned int>(entityId), std::move(newComponent));
    entityComponentSignatures[entityId].set(componentId);
}

template <typename TComponent>
void Registry::removeComponent(Entity entity)
{
    const int componentId = Component<TComponent>::getId();
    const int entityId = entity.getId();

    if (!isEntityValid(entity))
    {
        throw std::runtime_error("removeComponent: invalid entity");
    }

    if (static_cast<size_t>(componentId) >= componentPools.size() || !componentPools[componentId])
    {
        return;
    }

    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    if (static_cast<size_t>(entityId) >= componentPool->getSize())
    {
        return;
    }

    componentPool->set(static_cast<unsigned int>(entityId), TComponent{});
    entityComponentSignatures[entityId].set(componentId, false);

    removeSystemsEntity(entity);
    addSystemsEntity(entity);
}

template <typename TComponent>
bool Registry::hasComponent(Entity entity) const
{
    const int componentId = Component<TComponent>::getId();
    const int entityId = entity.getId();

    if (!isEntityValid(entity))
    {
        return false;
    }

    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent &Registry::getComponent(Entity entity) const
{
    const int componentId = Component<TComponent>::getId();
    const int entityId = entity.getId();

    if (!isEntityValid(entity))
    {
        throw std::runtime_error("getComponent: invalid entity");
    }

    if (static_cast<size_t>(componentId) >= componentPools.size() || !componentPools[componentId])
    {
        throw std::runtime_error("getComponent: component pool not found");
    }

    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    if (static_cast<size_t>(entityId) >= componentPool->getSize())
    {
        throw std::runtime_error("getComponent: entity index out of range");
    }

    return componentPool->get(static_cast<unsigned int>(entityId));
}

// System
template <typename TSystem, typename... TArgs>
void Registry::addSystem(TArgs &&...args)
{
    std::shared_ptr<TSystem> new_system = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), new_system));
}

template <typename TSystem>
void Registry::removeSystem()
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    if (system != systems.end())
    {
        systems.erase(system);
    }
}

template <typename TSystem>
bool Registry::hasSystem() const
{
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem &Registry::getSystem() const
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    if (system == systems.end())
    {
        throw std::runtime_error("getSystem: system not found");
    }
    return *(std::static_pointer_cast<TSystem>(system->second));
}

// Entity Add component
template <typename TComponent, typename... TArgs>
void Entity::addComponent(TArgs &&...args)
{
    if (!registry)
    {
        throw std::runtime_error("Entity::addComponent: null registry");
    }
    registry->addComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

// Entity remove component
template <typename TComponent>
void Entity::removeComponent()
{
    if (!registry)
    {
        throw std::runtime_error("Entity::removeComponent: null registry");
    }
    registry->removeComponent<TComponent>(*this);
}

// Entity has component
template <typename TComponent>
bool Entity::hasComponent() const
{
    if (!registry)
    {
        return false;
    }
    return registry->hasComponent<TComponent>(*this);
}

// Entity get component
template <typename TComponent>
TComponent &Entity::getComponent() const
{
    if (!registry)
    {
        throw std::runtime_error("Entity::getComponent: null registry");
    }
    return registry->getComponent<TComponent>(*this);
}

#endif // ECS_HPP