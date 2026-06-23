#pragma once

#include "ECS/component_array.hpp"

#include <unordered_map>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <cassert>

template<typename First, typename... Other> 
class View;

class ComponentManager {
public:
    template<typename T> void registerComponent(Entity max_components = MAX_ENTITIES);
    template<typename T> ComponentType getComponentType() const;
    template<typename... Args> Signature getSignature() const;
    
    template<typename T> void addComponent(Entity entity, T component);
    template<typename T> void modifyComponent(Entity entity, T new_data);
    template<typename T> void removeComponent(Entity entity);

    template<typename T, typename Compare> void sort(Compare comp);
    template<typename T, typename Compare> void sortFull(Compare comp);

    template<typename T> T& getComponent(Entity entity);
    template<typename T> const T& getComponent(Entity entity) const;

    void destroyEntity(Entity entity);
private:
    std::unordered_map<std::type_index, ComponentType> m_component_types{};
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> m_component_arrays{};
    ComponentType m_next_component_type = 0;

    template<typename T> std::shared_ptr<ComponentArray<T>> getComponentArray() const;
    template<typename First, typename... Other> friend class View;
};


template<typename T>
void ComponentManager::registerComponent(Entity max_components) {
    std::type_index typeName = std::type_index(typeid(T));
    assert(m_component_types.find(typeName) == m_component_types.end() && "Registering component type more than once.");

    m_component_types.insert({typeName, m_next_component_type});
    m_component_arrays.insert({typeName, std::make_shared<ComponentArray<T>>(max_components)});
    ++m_next_component_type;
}

template<typename T>
ComponentType ComponentManager::getComponentType() const{
    std::type_index typeName = std::type_index(typeid(T));
    assert(m_component_types.find(typeName) != m_component_types.end() && "Component not registered before use.");

    return m_component_types.at(typeName);
}

template<typename... Args>
Signature ComponentManager::getSignature() const{
    Signature signature;
    (signature.set(getComponentType<Args>(), true), ...);
    return signature;
}













template<typename T>
void ComponentManager::addComponent(Entity entity, T component) {
    this->getComponentArray<T>()->insertData(entity, std::move(component));
}
template<typename T>
void ComponentManager::modifyComponent(Entity entity, T new_data){
    this->getComponentArray<T>()->modifyData(entity,std::move(new_data));
}

template<typename T>
void ComponentManager::removeComponent(Entity entity) {
    this->getComponentArray<T>()->removeData(entity);
}



template<typename T, typename Compare>
void ComponentManager::sort(Compare comp) {
    getComponentArray<T>()->sort(comp);
}
 
template<typename T, typename Compare>
void ComponentManager::sortFull(Compare comp) {
    getComponentArray<T>()->sortFull(comp);
}





template<typename T>
T& ComponentManager::getComponent(Entity entity) {
    return this->getComponentArray<T>()->getData(entity);
}
template<typename T>
const T& ComponentManager::getComponent(Entity entity) const {
    return this->getComponentArray<T>()->getData(entity);
}







template<typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::getComponentArray() const{
    std::type_index typeName = std::type_index(typeid(T));
    assert(m_component_types.find(typeName) != m_component_types.end() && "Component not registered before use.");

    return std::static_pointer_cast<ComponentArray<T>>(m_component_arrays.at(typeName));
}