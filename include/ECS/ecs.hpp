#pragma once

#include "ECS/type.hpp"
#include "ECS/entity_manager.hpp"
#include "ECS/component_manager.hpp"

#include "ECS/view.hpp"

#include <memory>

template<typename T>
struct Comp {
    size_t capacity = MAX_ENTITIES; // Defaults to the massive array size
};

class ECS{
public:
    template<typename... Args> 
    void init();

    Entity createEntity();
    void destroyEntity(Entity entity);
    Signature getSignature(Entity entity);

    bool hasComponent(Entity entity, ComponentType type);
    template<typename T> bool hasComponent(Entity entity);
    bool hasSignature(Entity entity, Signature sign);
    template<typename... Args> bool hasSignature(Entity entity);



    template<typename T> void registerComponent(Entity max_components = MAX_ENTITIES);
    template<typename T> void addComponent(Entity entity, T data);         // Makes one copy and them moves it through, or fully moves if R alue ref passed
    template<typename T> void modifyData(Entity entity,T new_data);       
    
    template<typename T> void removeComponent(Entity entity);

    template<typename T, typename Compare> void sort(Compare comp);

    template<typename T> T& getComponent(Entity entity); // Querry T, you get T&, Querry const T, you get const T&
    template<typename T> T& getComponent(Entity entity) const; // Querry T, crash, Querry const T, you get const T&, enforcing the const in queery

    template<typename T> ComponentType getComponentType();
    template<typename... Args> Signature getSignature();


    template<typename FirstComp, typename... OtherComps> View<FirstComp, OtherComps...> getView();
    template<typename FirstComp, typename... OtherComps> friend class View;
private:
    EntityManager m_entity_manager;
    ComponentManager m_component_manager;

};

#include "ECS/view.ipp"

template<typename... Args> 
void ECS::init() {
    (this->registerComponent<Args>(), ...);
}

inline Entity ECS::createEntity(){
    return m_entity_manager.createEntity();
}
inline void ECS::destroyEntity(Entity entity){

    m_component_manager.destroyEntity(entity);
    m_entity_manager.destroyEntity(entity);
}
inline Signature ECS::getSignature(Entity entity){
    return m_entity_manager.getSignature(entity);
}


inline bool ECS::hasComponent(Entity entity, ComponentType type){
    return m_entity_manager.hasComponent(entity, type);
}
template<typename T> 
bool ECS::hasComponent(Entity entity){
    return m_entity_manager.hasComponent(entity, m_component_manager.getComponentType<T>());
}
inline bool ECS::hasSignature(Entity entity, Signature sign){
    return ((m_entity_manager.getSignature(entity)&sign) == sign);
}
template<typename... Args> 
bool ECS::hasSignature(Entity entity){
    return hasSignature(entity, m_component_manager.getSignature<Args...>() );
}









template<typename T> 
void ECS::registerComponent(Entity max_components){
    m_component_manager.registerComponent<T>(max_components);
}

template<typename T> 
void ECS::addComponent(Entity entity, T data){
    ComponentType type = m_component_manager.getComponentType<T>();
    m_component_manager.addComponent(entity,std::move(data));
    Signature newSign = m_entity_manager.addComponentType(entity, type);
}
template<typename T> 
void ECS::modifyData(Entity entity,T new_data){
    m_component_manager.modifyComponent(entity,std::move(new_data));
}
template<typename T> 
void ECS::removeComponent(Entity entity){
    ComponentType type = m_component_manager.getComponentType<T>();
    m_component_manager.removeComponent<T>(entity);  
    m_entity_manager.removeComponentType(entity, type);
}









template<typename T, typename Compare> 
void ECS::sort(Compare comp){
    m_component_manager.sort<T>(comp);
}





// 1. The Mutable ECS Overload
template<typename T> 
T& ECS::getComponent(Entity entity) {
    if constexpr (std::is_const_v<T>) {
        return std::as_const(m_component_manager).template getComponent<std::remove_const_t<T>>(entity);
    } else {
        return m_component_manager.template getComponent<std::remove_const_t<T>>(entity);
    }
}

// 2. The Read-Only (const) ECS Overload
template<typename T> 
T& ECS::getComponent(Entity entity) const {
    static_assert(std::is_const_v<T>, "ERROR: You are querying a const ECS. You must explicitly request a const component (e.g., getComponent<const Position>()).");
    return m_component_manager.template getComponent<std::remove_const_t<T>>(entity);
}




template<typename T> ComponentType ECS::getComponentType(){
    return m_component_manager.getComponentType<T>();
}
template<typename... Args> Signature ECS::getSignature(){
    return m_component_manager.getSignature<Args...>();
}




template<typename FirstComp, typename... OtherComps> 
View<FirstComp, OtherComps...> ECS::getView(){
    return View<FirstComp, OtherComps...>(*this);
}