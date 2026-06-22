#pragma once

#include "ECS/type.hpp"
#include "ECS/entity_manager.hpp"
#include "ECS/component_manager.hpp"
#include "ECS/system_manager.hpp"

#include <memory>

class ECS{
public:
    template<typename... Args> 
    void init();

    Entity createEntity();
    void destroyEntity(Entity entity);
    Signature getSignature(Entity entity);



    template<typename T> void registerComponent();
    template<typename T> void addComponent(Entity entity, T data);
    template<typename T> void modifyData(Entity entity,T new_data);       // Makes one copy and them moves it through, or fully moves if R alue ref passed
    template<typename T> bool hasComponent(Entity entity);
    template<typename T> void removeComponent(Entity entity);

    template<typename T> void sort(std::function<bool(const T&, const T&)> comp);
    template<typename T> void sort();

    template<typename T> T& getComponent(Entity entity);
    template<typename T> const T& getComponent(Entity entity) const;

    template<typename T> ComponentType getComponentType();
    template<typename... Args> Signature getSignature();


    template<typename T>  std::shared_ptr<T> createSystem();
private:
    EntityManager m_entity_manager;
    ComponentManager m_component_manager;
    SystemManager m_system_manager;
};


template<typename... Args> 
void ECS::init() {
    (this->registerComponent<Args>(), ...);
}



template<typename T> 
void ECS::registerComponent(){
    m_component_manager.registerComponent<T>();
}

template<typename T> 
void ECS::addComponent(Entity entity, T data){
    ComponentType type = m_component_manager.getComponentType<T>();
    Signature newSign = m_entity_manager.addComponentType(entity, type);
    m_component_manager.addComponent(entity,std::move(data));
    m_system_manager.addComponent(entity,newSign,type);
}
template<typename T> 
void ECS::modifyData(Entity entity,T new_data){
    m_component_manager.modifyComponent(entity,std::move(new_data));
}
template<typename T> 
bool ECS::hasComponent(Entity entity){
    return m_entity_manager.hasComponent(entity, m_component_manager.getComponentType<T>());
}
template<typename T> 
void ECS::removeComponent(Entity entity){
    ComponentType type = m_component_manager.getComponentType<T>();
    m_entity_manager.removeComponentType(entity, type);
    m_component_manager.removeComponent<T>(entity);  
    m_system_manager.removeComponent(entity,type);
}



template<typename T> 
void ECS::sort(std::function<bool(const T&, const T&)> comp){
    m_component_manager.sort(comp);
}
template<typename T> 
void ECS::sort(){
    m_component_manager.sort<T>();
}




template<typename T> 
T& ECS::getComponent(Entity entity){
    return m_component_manager.getComponent<T>(entity);
}
template<typename T> 
const T& ECS::getComponent(Entity entity) const{
    return m_component_manager.getComponent<T>(entity);
}




template<typename T> ComponentType ECS::getComponentType(){
    return m_component_manager.getComponentType<T>();
}
template<typename... Args> Signature ECS::getSignature(){
    return m_component_manager.getSignature<Args...>();
}


template<typename T>
std::shared_ptr<T> ECS::createSystem(){
    static_assert(std::is_constructible_v<T, ECS&>, "ERROR: The System type 'T' must have a constructor that accepts an 'ECS&'!");
    std::shared_ptr<T> sys = std::make_shared<T>(*this);
    m_system_manager.registerSystem(sys);
    return sys;
}