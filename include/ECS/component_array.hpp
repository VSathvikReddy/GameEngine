#pragma once

#include "ECS/type.hpp"

#include <cassert>
#include <unordered_map>
#include <array>
#include <functional>
#include <numeric>
#include <algorithm>

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
    ComponentArray();  // Fills the sparse array and sort buffer

    void insertData(Entity entity, T component);    // Inerts at the right place (or end)
    void modifyData(Entity entity, T new_data);     // Modifies the data and puts it in the right place
    void removeData(Entity entity);                 // Removes tha data and moves all to right (or swap and pop)

    T& getData(Entity entity);                      // Only to be used when the modification doesnt effect order, (or call sort after using)
    const T& getData(Entity entity) const;
    void destroyEntity(Entity entity) override;     // remove data if entity exists

    bool setCompareFunc(std::function<bool(const T&, const T&)> comp);   // Set function and call sort
    void sort();                                   

    size_t size() const;

private:
    // Does not allow for components without default constructor, EnTT allocated raw memory istead of this
    std::array<T, MAX_ENTITIES> m_dense_component_array;

    std::array<Entity, MAX_ENTITIES> m_dense_entity_array;
    std::array<size_t, MAX_ENTITIES> m_sparse_index_array;

    size_t m_size = 0; 
    static constexpr size_t NULL_INDEX = static_cast<size_t>(-1);

    std::function<bool(const T&, const T&)> m_comparator;
    std::vector<size_t> m_sort_buffer;

    template<typename First, typename... Other> friend class View;
};




template<typename T>
ComponentArray<T>::ComponentArray() {
    m_sparse_index_array.fill(NULL_INDEX);
    m_sort_buffer.reserve(MAX_ENTITIES);
}


template<typename T>
void ComponentArray<T>::insertData(Entity entity, T component) {
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(m_sparse_index_array[entity] == NULL_INDEX && "Component added to same entity more than once.");

    size_t insertIndex = m_size;

    if (m_comparator) {
        while (insertIndex > 0 && m_comparator(component, m_dense_component_array[insertIndex - 1])) {
            
            m_dense_component_array[insertIndex] = std::move(m_dense_component_array[insertIndex - 1]);
            m_dense_entity_array[insertIndex] = m_dense_entity_array[insertIndex - 1];
            
            m_sparse_index_array[m_dense_entity_array[insertIndex]] = insertIndex;

            --insertIndex;
        }
    }

    m_dense_component_array[insertIndex] = std::move(component);
    m_dense_entity_array[insertIndex] = entity;
    m_sparse_index_array[entity] = insertIndex;
    
    ++m_size;
}

template<typename T>
void ComponentArray<T>::modifyData(Entity entity, T new_data) {
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(m_sparse_index_array[entity] != NULL_INDEX && "Modifying non-existent component.");

    size_t index = m_sparse_index_array[entity];
    
    m_dense_component_array[index] = std::move(new_data);

    if (m_comparator){
        
        // Move left
        while (index > 0 && m_comparator(m_dense_component_array[index], m_dense_component_array[index - 1])) {
            // Swap dense components
            std::swap(m_dense_component_array[index], m_dense_component_array[index - 1]);
            // Swap dense entities
            std::swap(m_dense_entity_array[index], m_dense_entity_array[index - 1]);
            
            // Update sparse arrays
            m_sparse_index_array[m_dense_entity_array[index]] = index;
            m_sparse_index_array[m_dense_entity_array[index - 1]] = index - 1;
            
            --index;
        }

        // Move right
        while (index < m_size - 1 && m_comparator(m_dense_component_array[index + 1], m_dense_component_array[index])) {
            // Swap dense components
            std::swap(m_dense_component_array[index], m_dense_component_array[index + 1]);
            // Swap dense entities
            std::swap(m_dense_entity_array[index], m_dense_entity_array[index + 1]);
            
            // Update sparse arrays
            m_sparse_index_array[m_dense_entity_array[index]] = index;
            m_sparse_index_array[m_dense_entity_array[index + 1]] = index + 1;
            
            ++index;
        }
    }
}

template<typename T>
void ComponentArray<T>::removeData(Entity entity) {
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(m_sparse_index_array[entity] != NULL_INDEX && "Removing non-existent component.");

    size_t removedIndex = m_sparse_index_array[entity];

    if (m_comparator) {
        for (size_t i = removedIndex; i < m_size - 1; ++i) {
            m_dense_component_array[i] = std::move(m_dense_component_array[i + 1]);
            m_dense_entity_array[i] = m_dense_entity_array[i + 1];
            
            m_sparse_index_array[m_dense_entity_array[i]] = i;
        }
    } else {
        size_t lastIndex = m_size - 1;
        
        m_dense_component_array[removedIndex] = std::move(m_dense_component_array[lastIndex]);
        m_dense_entity_array[removedIndex] = m_dense_entity_array[lastIndex];
        
        m_sparse_index_array[m_dense_entity_array[removedIndex]] = removedIndex;
    }

    m_sparse_index_array[entity] = NULL_INDEX;
    --m_size;
}

template<typename T>
T& ComponentArray<T>::getData(Entity entity) {
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    assert(m_sparse_index_array[entity] != NULL_INDEX && "Retrieving non-existent component.");

    return m_dense_component_array[m_sparse_index_array[entity]];
}
template<typename T>
const T& ComponentArray<T>::getData(Entity entity) const{
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
bool ComponentArray<T>::setCompareFunc(std::function<bool(const T&, const T&)> comp) {
    bool wasOverride = static_cast<bool>(m_comparator);
    m_comparator = comp;

    if (m_size > 1 && m_comparator){
        sort();
    }

    return !wasOverride;
}

template<typename T>
void ComponentArray<T>::sort(){
    assert( m_comparator && "Calling sort without existing sort func");
    m_sort_buffer.resize(m_size);
    std::iota(m_sort_buffer.begin(), m_sort_buffer.end(), 0);

    std::sort(m_sort_buffer.begin(), m_sort_buffer.end(), [&](size_t a, size_t b) {
        return m_comparator(m_dense_component_array[a], m_dense_component_array[b]);
    });

    // Cycle sort
    for (size_t i = 0; i < m_size; ++i) {
        if (i != m_sort_buffer[i]) {
            size_t current = i;
            size_t next = m_sort_buffer[current];

            T temp_comp = std::move(m_dense_component_array[current]);
            Entity temp_entity = m_dense_entity_array[current];

            // Follow the cycle
            while (next != i) {
                m_dense_component_array[current] = std::move(m_dense_component_array[next]);
                m_dense_entity_array[current] = m_dense_entity_array[next];
                
                m_sort_buffer[current] = current; 
                current = next;
                next = m_sort_buffer[next];
            }

            m_dense_component_array[current] = std::move(temp_comp);
            m_dense_entity_array[current] = temp_entity;
            m_sort_buffer[current] = current; 
        }
    }

    // Rebuild sparse array
    for (size_t i = 0; i < m_size; ++i) {
        m_sparse_index_array[m_dense_entity_array[i]] = i;
    }
}

template<typename T>
size_t ComponentArray<T>::size() const{
    return this->m_size;
}