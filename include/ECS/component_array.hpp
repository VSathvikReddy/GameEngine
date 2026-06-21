#pragma once

#include "ECS/type.hpp"

#include <cassert>
#include <unordered_map>
#include <array>

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void destroyEntity(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray {
public:
    void insertData(Entity entity, T component);
    void removeData(Entity entity);
    T& getData(Entity entity);
    const T& getData(Entity entity) const;
    void destroyEntity(Entity entity) override;

private:
    std::array<T, MAX_ENTITIES> m_component_array;
    std::unordered_map<Entity, size_t> m_entity_to_index_map;
    std::unordered_map<size_t, Entity> m_index_to_entity_map;
    size_t m_size = 0; 
};







template<typename T>
void ComponentArray<T>::insertData(Entity entity, T component) {
    assert(m_entity_to_index_map.find(entity) == m_entity_to_index_map.end() && "Component added to same entity more than once.");

    size_t newIndex = m_size;
    m_entity_to_index_map[entity] = newIndex;
    m_index_to_entity_map[newIndex] = entity;
    m_component_array[newIndex] = component;
    ++m_size;
}

template<typename T>
void ComponentArray<T>::removeData(Entity entity) {
    assert(m_entity_to_index_map.find(entity) != m_entity_to_index_map.end() && "Removing non-existent component.");

    size_t indexOfRemovedEntity = m_entity_to_index_map[entity];
    size_t indexOfLastElement = m_size - 1;
    m_component_array[indexOfRemovedEntity] = m_component_array[indexOfLastElement];

    Entity entityOfLastElement = m_index_to_entity_map[indexOfLastElement];
    m_entity_to_index_map[entityOfLastElement] = indexOfRemovedEntity;
    m_index_to_entity_map[indexOfRemovedEntity] = entityOfLastElement;

    m_entity_to_index_map.erase(entity);
    m_index_to_entity_map.erase(indexOfLastElement);

    --m_size;
}

template<typename T>
T& ComponentArray<T>::getData(Entity entity) {
    assert(m_entity_to_index_map.find(entity) != m_entity_to_index_map.end() && "Retrieving non-existent component.");
    return m_component_array[m_entity_to_index_map[entity]];
}
template<typename T>
const T& ComponentArray<T>::getData(Entity entity) const{
    assert(m_entity_to_index_map.find(entity) != m_entity_to_index_map.end() && "Retrieving non-existent component.");
    return m_component_array[m_entity_to_index_map.at(entity)];
}

template<typename T>
void ComponentArray<T>::destroyEntity(Entity entity) {
    if (m_entity_to_index_map.find(entity) != m_entity_to_index_map.end()) {
        removeData(entity);
    }
}