#pragma once

#include "ECS/type.hpp" // Assuming this defines Entity, MAX_ENTITIES, etc.

#include <cassert>
#include <vector>
#include <array>
#include <numeric>
#include <algorithm>
#include <utility>

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void destroyEntity(Entity entity) = 0;
};
template<typename First, typename... Other> 
class View;

template<typename T>
class ComponentArray : public IComponentArray {
public:
    explicit ComponentArray(Entity max_component_count = MAX_ENTITIES); 

    void insertData(Entity entity, T component); 
    void modifyData(Entity entity, T new_data);  
    void removeData(Entity entity);              

    T& getData(Entity entity);                   
    const T& getData(Entity entity) const;


    void destroyEntity(Entity entity) override;  
    size_t size() const;

    template<typename Compare>
    void sort(Compare comp); 
private:
    std::vector<T> m_dense_component_array;
    std::vector<Entity> m_dense_entity_array;

    std::array<size_t, MAX_ENTITIES> m_sparse_index_array;

    size_t m_size = 0; 
    size_t m_max_capacity = 0;
    static constexpr size_t NULL_INDEX = static_cast<size_t>(-1);

    template<typename First, typename... Other> friend class View;
};

// ------------------------------------------------------------------
// Implementation (.inl)
// ------------------------------------------------------------------

template<typename T>
ComponentArray<T>::ComponentArray(Entity max_component_count):m_max_capacity(max_component_count) {
    m_sparse_index_array.fill(NULL_INDEX);
    

    m_dense_component_array.reserve(max_component_count);
    m_dense_entity_array.reserve(max_component_count);
}

template<typename T>
void ComponentArray<T>::insertData(Entity entity, T component) {
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(m_sparse_index_array[entity] == NULL_INDEX && "Component added to same entity more than once.");
    assert(m_size < m_max_capacity && "Exceeded maximum component capacity! Vector will reallocate.");

    m_dense_component_array.push_back(std::move(component));
    m_dense_entity_array.push_back(entity);
    
    m_sparse_index_array[entity] = m_size;
    ++m_size;
}

template<typename T>
void ComponentArray<T>::modifyData(Entity entity, T new_data) {
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(m_sparse_index_array[entity] != NULL_INDEX && "Modifying non-existent component.");

    size_t index = m_sparse_index_array[entity];
    m_dense_component_array[index] = std::move(new_data);
}

template<typename T>
void ComponentArray<T>::removeData(Entity entity) {
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(m_sparse_index_array[entity] != NULL_INDEX && "Removing non-existent component.");

    // O(1) Swap and Pop
    size_t removedIndex = m_sparse_index_array[entity];
    size_t lastIndex = m_size - 1;

    // If it's not the last element, swap the last element into the deleted slot
    if (removedIndex != lastIndex) {
        m_dense_component_array[removedIndex] = std::move(m_dense_component_array[lastIndex]);
        m_dense_entity_array[removedIndex] = m_dense_entity_array[lastIndex];
        
        // Update the sparse map for the entity we just moved
        Entity movedEntity = m_dense_entity_array[removedIndex];
        m_sparse_index_array[movedEntity] = removedIndex;
    }

    m_sparse_index_array[entity] = NULL_INDEX;

    m_dense_component_array.pop_back();
    m_dense_entity_array.pop_back();

    --m_size;
}

template<typename T>
T& ComponentArray<T>::getData(Entity entity) {
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(m_sparse_index_array[entity] != NULL_INDEX && "Retrieving non-existent component.");
    return m_dense_component_array[m_sparse_index_array[entity]];
}

template<typename T>
const T& ComponentArray<T>::getData(Entity entity) const {
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(m_sparse_index_array[entity] != NULL_INDEX && "Retrieving non-existent component.");
    return m_dense_component_array[m_sparse_index_array[entity]];
}

template<typename T>
void ComponentArray<T>::destroyEntity(Entity entity) {
    if (entity < MAX_ENTITIES && m_sparse_index_array[entity] != NULL_INDEX) {
        removeData(entity);
    }
}

template<typename T>
size_t ComponentArray<T>::size() const {
    return m_size;
}



template<typename T>
template<typename Compare>
void ComponentArray<T>::sort(Compare comp) {
    if (m_size <= 1) return;

    // Build a permutation index sorted by comp
    std::vector<size_t> perm(m_size);
    std::iota(perm.begin(), perm.end(), 0);
    std::sort(perm.begin(), perm.end(), [&](size_t a, size_t b) {
        return comp(m_dense_component_array[a], m_dense_component_array[b]);
    });

    // Apply the permutation in-place using cycle decomposition
    std::vector<bool> visited(m_size, false);

    for (size_t i = 0; i < m_size; ++i) {
        if (visited[i] || perm[i] == i) {
            visited[i] = true;
            continue;
        }

        // Walk the cycle starting at i
        size_t cycle_start = i;
        T    temp_comp   = std::move(m_dense_component_array[i]);
        Entity temp_entity = m_dense_entity_array[i];

        size_t current = i;
        while (perm[current] != cycle_start) {
            size_t next = perm[current];

            m_dense_component_array[current] = std::move(m_dense_component_array[next]);
            m_dense_entity_array[current]    = m_dense_entity_array[next];

            visited[current] = true;
            current = next;
        }

        // Place the saved head element at the cycle's tail slot
        m_dense_component_array[current] = std::move(temp_comp);
        m_dense_entity_array[current]    = temp_entity;
        visited[current] = true;
    }

    // Rebuild sparse index from the (now correct) dense entity array
    for (size_t i = 0; i < m_size; ++i) {
        m_sparse_index_array[m_dense_entity_array[i]] = i;
    }
}