#include "ecs.hpp"

int IComponent::nextId = 0;

int Entity::getId() const {
    return id;
}

std::string Entity::getName() const {
    return name;
}

void System::addSystemEntity(Entity entity) {
    entities.push_back(entity);
}

void System::removeSystemEntity(Entity entity) {
    auto it = std::remove_if(entities.begin(), entities.end(), \
        [&entity](Entity other) { return entity == other;} );
    
    entities.erase(it, entities.end());
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
    registry->destroyEntity(*this);
}

Entity Registry::createEntity(std::string name = "") {
    int entity_id;

    if (freeIds.empty()) {
        entity_id = numEntity++;

        if (static_cast<long unsigned int>(entity_id) >= entityComponentSignatures.size()) {
        entityComponentSignatures.resize(entity_id + 100); // Increases the size by 100
    }
    } else { // Not empty, use an existing value
        entity_id = freeIds.front();
        freeIds.pop_front(); // Use the value and remove it
    }

    Entity entity(entity_id, name);
    entity.registry = this;
    entitiesToAdd.insert(entity);
    
    std::cout << "[REGISTRY] Entity " << entity_id << " created succesfully!" << std::endl;
    return entity;
}

void Registry::destroyEntity(Entity entity) {
    entitiesToDelete.insert(entity);
}

void Registry::addSystemsEntity(Entity entity) {
    const int entityId =entity.getId();

    const signature& entityComponentSignature \
        = entityComponentSignatures[entityId];

    for (auto& system : systems) {
        const auto& systemComponentSignature \
            = system.second->getComponentSignature();

        bool is_interested = (entityComponentSignature & systemComponentSignature) \
            == systemComponentSignature;

        if (is_interested) {
            system.second->addSystemEntity(entity);
        }
    }
}

void Registry::removeSystemsEntity(Entity entity) {
    for (auto system : systems) {
        system.second->removeSystemEntity(entity);
    }
}

void Registry::update() {
    for (auto entity : entitiesToAdd) {
        addSystemsEntity(entity);
    }

    entitiesToAdd.clear();

    for (auto& entity : entitiesToDelete) {
        removeSystemsEntity(entity);
        entityComponentSignatures[entity.getId()].reset();

        freeIds.push_back(entity.getId());
    }

    entitiesToDelete.clear();
}