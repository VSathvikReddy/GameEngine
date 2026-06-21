#include "ECS/entity_manager.hpp"

#include <cassert>

EntityManager::EntityManager() {
    for (Entity e = MAX_ENTITIES; e > 0; --e) {
        m_available_entities.push(e-1);
    }
}

Entity EntityManager::createEntity(){
    assert(m_living_entity_count < MAX_ENTITIES && "Too many entities in existence.");

    // Take an ID from the front of the queue
    Entity id = m_available_entities.top();
    m_available_entities.pop();
    ++m_living_entity_count;

    return id;
}

void EntityManager::destroyEntity(Entity entity){
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    m_signatures[entity].reset();

    // Put the destroyed ID at the back of the queue
    m_available_entities.push(entity);
    --m_living_entity_count;
}

Signature EntityManager::getSignature(Entity entity){
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    return m_signatures[entity];
}
Signature EntityManager::setSignature(Entity entity, Signature sign){
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    return (m_signatures[entity] = sign);
}

bool EntityManager::hasComponent(Entity entity, ComponentType type){
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    return m_signatures[entity].test(type);
}


Signature EntityManager::addComponentType(Entity entity, ComponentType type){
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(!m_signatures[entity].test(type) && "Component added to entity more than once.");
    return m_signatures[entity].set(type, true);
}
Signature EntityManager::removeComponentType(Entity entity, ComponentType type){
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(m_signatures[entity].test(type) && "Component removed from entity that did not have it.");
    return m_signatures[entity].set(type, false);
}


