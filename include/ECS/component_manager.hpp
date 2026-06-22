#pragma once

#include "ECS/component_array.hpp"

#include <unordered_map>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <cassert>


class ComponentManager {
public:
    template<typename T> void registerComponent();
    template<typename T> ComponentType getComponentType();
    template<typename... Args> Signature getSignature();
    
    template<typename T> void addComponent(Entity entity, T component);
    template<typename T> void modifyComponent(Entity entity, T new_data);
    template<typename T> void removeComponent(Entity entity);

    template<typename T> void sort(std::function<bool(const T&, const T&)> comp);
    template<typename T> void sort();

    template<typename T> T& getComponent(Entity entity);
    template<typename T> const T& getComponent(Entity entity) const;

    void destroyEntity(Entity entity);
private:
    std::unordered_map<std::type_index, ComponentType> m_component_types{};
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> m_component_arrays{};
    ComponentType m_next_component_type = 0;

    template<typename T> std::shared_ptr<ComponentArray<T>> getComponentArray() const;
};


template<typename T>
void ComponentManager::registerComponent() {
    std::type_index typeName = std::type_index(typeid(T));
    assert(m_component_types.find(typeName) == m_component_types.end() && "Registering component type more than once.");

    m_component_types.insert({typeName, m_next_component_type});
    m_component_arrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
    ++m_next_component_type;
}

template<typename T>
ComponentType ComponentManager::getComponentType() {
    std::type_index typeName = std::type_index(typeid(T));
    assert(m_component_types.find(typeName) != m_component_types.end() && "Component not registered before use.");

    return m_component_types[typeName];
}

template<typename... Args>
Signature ComponentManager::getSignature() {
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




template<typename T> 
void ComponentManager::sort(std::function<bool(const T&, const T&)> comp){
    this->getComponentArray<T>()->setCompareFunc(comp); // Can return
}
template<typename T> 
void ComponentManager::sort(){
    this->getComponentArray<T>()->sort();
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