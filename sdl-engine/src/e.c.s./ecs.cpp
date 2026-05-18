#include "ecs.hpp"

int IComponent::nextId = 0;

EntityIndex Entity::getId() const {
    return index;
}

EntityIndex Entity::getIndex() const {
    return index;
}

EntityGeneration Entity::getGeneration() const {
    return generation;
}

std::string Entity::getName() const {
    return name;
}

void System::addSystemEntity(Entity entity) {
    auto it = std::find_if(entities.begin(), entities.end(),
        [&entity](const Entity& other) {
            return entity.getId() == other.getId();
        });

    if (it == entities.end()) {
        entities.push_back(entity);
    }
}

void System::removeSystemEntity(Entity entity) {
    auto it = std::remove_if(entities.begin(), entities.end(),
        [&entity](const Entity& other) {
            return entity.getId() == other.getId();
        });
    
    entities.erase(it, entities.end());
}

void System::clearEntities() {
    entities.clear();
}

std::vector<Entity> System::getSystemEntities() const {
    return entities;
}

const signature& System::getComponentSignature() const {
    return componentSignature;
}

Registry::Registry() {
    std::cout << "[REGISTRY] Executes constructor!" << std::endl; 
}

Registry::~Registry() {
    std::cout << "[REGISTRY] Executes destructor!" << std::endl;
}

void Entity::kill() {
    if (registry) {
        registry->destroyEntity(*this);
    }
}

Entity Registry::createEntity() {
    EntityIndex entityIndex;
    EntityGeneration entityGeneration = 0;

    if (!freeIndices.empty()) {
        entityIndex = freeIndices.front();
        freeIndices.pop_front();
        entityGeneration = entityGenerations[entityIndex];
    } else {
        entityIndex = numEntity++;
        if (static_cast<size_t>(entityIndex) >= entityGenerations.size()) {
            entityGenerations.resize(entityIndex + 100, 0);
        }
        entityGeneration = entityGenerations[entityIndex];
    }

    if (static_cast<size_t>(entityIndex) >= entityComponentSignatures.size()) {
        entityComponentSignatures.resize(entityIndex + 100);
    }

    Entity entity(entityIndex, entityGeneration);
    entity.registry = this;
    entitiesToAdd.insert(entity);
    
    std::cout << "[REGISTRY] Entity " << entityIndex << " created successfully!" << std::endl;
    return entity;
}

void Registry::destroyEntity(Entity entity) {
    if (!isEntityValid(entity)) {
        return;
    }

    entitiesToDelete.insert(entity);
    unregisterEntityName(entity);
}

bool Registry::isEntityValid(Entity entity) const {
    if (entity.registry != this) {
        return false;
    }

    EntityIndex entityIndex = entity.getIndex();
    if (entityIndex < 0 || static_cast<size_t>(entityIndex) >= entityGenerations.size()) {
        return false;
    }

    return entity.getGeneration() == entityGenerations[entityIndex];
}

void Registry::unregisterEntityName(Entity entity) {
    for (auto it = namedEntities.begin(); it != namedEntities.end();) {
        if (it->second.getId() == entity.getId() && it->second.getGeneration() == entity.getGeneration()) {
            it = namedEntities.erase(it);
        } else {
            ++it;
        }
    }
}

void Registry::addSystemsEntity(Entity entity) {
    if (!isEntityValid(entity)) {
        return;
    }

    EntityIndex entityId = entity.getId();
    if (static_cast<size_t>(entityId) >= entityComponentSignatures.size()) {
        return;
    }

    const signature& entityComponentSignature = entityComponentSignatures[entityId];

    for (auto& systemPair : systems) {
        const auto& systemComponentSignature = systemPair.second->getComponentSignature();

        bool is_interested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
        if (is_interested) {
            systemPair.second->addSystemEntity(entity);
        }
    }
}

void Registry::removeSystemsEntity(Entity entity) {
    for (auto& systemPair : systems) {
        systemPair.second->removeSystemEntity(entity);
    }
}

void Registry::update() {
    for (auto entity : entitiesToAdd) {
        if (isEntityValid(entity)) {
            addSystemsEntity(entity);
        }
    }
    entitiesToAdd.clear();

    for (auto& entity : entitiesToDelete) {
        removeSystemsEntity(entity);
        EntityIndex entityIndex = entity.getIndex();

        if (static_cast<size_t>(entityIndex) < entityComponentSignatures.size()) {
            entityComponentSignatures[entityIndex].reset();
        }

        if (static_cast<size_t>(entityIndex) < entityGenerations.size()) {
            entityGenerations[entityIndex]++;
        }

        freeIndices.push_back(entityIndex);
    }

    entitiesToDelete.clear();
}

void Registry::registerEntityName(const std::string& name, Entity entity) {
    namedEntities[name] = entity;
}

Entity Registry::findEntity(const std::string& name) const {
    auto it = namedEntities.find(name);
    if (it == namedEntities.end()) {
        throw std::runtime_error("Entity not found: " + name);
    }
    return it->second;
}

bool Registry::hasEntity(const std::string& name) const {
    return namedEntities.find(name) != namedEntities.end();
}

void Registry::clearAllEntities() {
    namedEntities.clear();
    entitiesToAdd.clear();
    entitiesToDelete.clear();
    freeIndices.clear();

    for (auto& systemPair : systems) {
        systemPair.second->clearEntities();
    }

    for (EntityIndex i = 0; i < numEntity; ++i) {
        if (static_cast<size_t>(i) < entityComponentSignatures.size()) {
            entityComponentSignatures[i].reset();
        }

        if (static_cast<size_t>(i) < entityGenerations.size()) {
            entityGenerations[i]++;
        }

        freeIndices.push_back(i);
    }

    componentPools.clear();
}
