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
    void init(); // For entites with MAX_ENTITES as capacity

    Entity createEntity();
    void destroyEntity(Entity entity);     // Destroys all the data regarding the entity immediatly
    Signature getSignature(Entity entity) const;    // Doesnt check for non-existant entity querries

    bool hasComponent(Entity entity, ComponentType type) const; // Same as above
    template<typename T> bool hasComponent(Entity entity) const;
    bool hasSignature(Entity entity, Signature sign) const;
    template<typename... Args> bool hasSignature(Entity entity) const;



    template<typename T> void registerComponent(Entity max_components = MAX_ENTITIES);  //Manual Registraiton for comonents with smaller count of instances
    template<typename T> void addComponent(Entity entity, T data);          // Makes one copy and them moves it through, or fully moves if R alue ref passed
    template<typename T> void modifyData(Entity entity,T new_data);         // Same
    template<typename T> void removeComponent(Entity entity);

    template<typename T, typename Compare> void sort(Compare comp);         // Insertion sort
    template<typename T, typename Compare> void sortFull(Compare comp);     // std::sort

    template<typename T> T& getComponent(Entity entity); // suppoerts get<T> and get<const T> to get T& and const T& respectively
    template<typename T> T& getComponent(Entity entity) const; // if ecs is const, only querry <const T> to get const T& otherwise throw eroor

    template<typename T> ComponentType getComponentType() const;
    template<typename... Args> Signature getSignature() const;

    template<typename T> size_t size() const;
    template<typename FirstComp, typename... OtherComps> View<FirstComp, OtherComps...> getView(); // Refer to view class
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
inline Signature ECS::getSignature(Entity entity) const{
    return m_entity_manager.getSignature(entity);
}


inline bool ECS::hasComponent(Entity entity, ComponentType type) const{
    return m_entity_manager.hasComponent(entity, type);
}
template<typename T> 
bool ECS::hasComponent(Entity entity) const{
    return m_entity_manager.hasComponent(entity, m_component_manager.getComponentType<T>());
}
inline bool ECS::hasSignature(Entity entity, Signature sign) const{
    return ((m_entity_manager.getSignature(entity)&sign) == sign);
}
template<typename... Args> 
bool ECS::hasSignature(Entity entity) const{
    return hasSignature(entity, m_component_manager.getSignature<Args...>() );
}









template<typename T> 
void ECS::registerComponent(Entity max_components){
    m_component_manager.registerComponent<T>(max_components);
}

template<typename T> 
void ECS::addComponent(Entity entity, T data){ // First compo manager then entity manager, incase error, no corruption
    m_component_manager.addComponent(entity,std::move(data));
    m_entity_manager.addComponentType(entity, m_component_manager.getComponentType<T>());
}
template<typename T> 
void ECS::modifyData(Entity entity,T new_data){
    m_component_manager.modifyComponent(entity,std::move(new_data));
}
template<typename T> 
void ECS::removeComponent(Entity entity){
    m_component_manager.removeComponent<T>(entity);  
    m_entity_manager.removeComponentType(entity, m_component_manager.getComponentType<T>());
}









template<typename T, typename Compare>
void ECS::sort(Compare comp) {
    m_component_manager.sort<T>(comp);
}
 
template<typename T, typename Compare>
void ECS::sortFull(Compare comp) {
    m_component_manager.sortFull<T>(comp);
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




template<typename T> ComponentType ECS::getComponentType() const{
    return m_component_manager.getComponentType<T>();
}
template<typename... Args> Signature ECS::getSignature() const{
    return m_component_manager.getSignature<Args...>();
}


template<typename T> 
size_t ECS::size() const{
    return m_component_manager.size<T>();
}

template<typename FirstComp, typename... OtherComps> 
View<FirstComp, OtherComps...> ECS::getView(){
    return View<FirstComp, OtherComps...>(*this);
}