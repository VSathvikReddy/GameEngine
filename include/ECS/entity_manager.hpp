#pragma once

#include "ECS/type.hpp"

#include <stack>
#include <array>

class EntityManager{
public:
    EntityManager();
    
    Entity createEntity();
    void destroyEntity(Entity entity);

    Signature getSignature(Entity entity) const;
    Signature setSignature(Entity entity, Signature sign);

    bool hasComponent(Entity entity, ComponentType type) const;
    Signature addComponentType(Entity entity, ComponentType type);
    Signature removeComponentType(Entity entity, ComponentType type);
private:
	std::stack<Entity> m_available_entities{};
	std::array<Signature, MAX_ENTITIES> m_signatures{};

	Entity m_living_entity_count = 0;
};


#include <cassert>

EntityManager::EntityManager() {
    for (Entity e = MAX_ENTITIES; e > 0; --e) {
        m_available_entities.push(e-1);
    }
}

inline Entity EntityManager::createEntity(){
    assert(m_living_entity_count < MAX_ENTITIES && "Too many entities in existence.");

    // Take an ID from the front of the queue
    Entity id = m_available_entities.top();
    m_available_entities.pop();
    ++m_living_entity_count;

    return id;
}

inline void EntityManager::destroyEntity(Entity entity){
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    m_signatures[entity].reset();

    // Put the destroyed ID at the back of the queue
    m_available_entities.push(entity);
    --m_living_entity_count;
}

inline Signature EntityManager::getSignature(Entity entity) const{
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    return m_signatures[entity];
}
inline Signature EntityManager::setSignature(Entity entity, Signature sign){
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    return (m_signatures[entity] = sign);
}

inline bool EntityManager::hasComponent(Entity entity, ComponentType type) const{
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    return m_signatures[entity].test(type);
}


inline Signature EntityManager::addComponentType(Entity entity, ComponentType type){
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(!m_signatures[entity].test(type) && "Component added to entity more than once.");
    return m_signatures[entity].set(type, true);
}
inline Signature EntityManager::removeComponentType(Entity entity, ComponentType type){
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(m_signatures[entity].test(type) && "Component removed from entity that did not have it.");
    return m_signatures[entity].set(type, false);
}