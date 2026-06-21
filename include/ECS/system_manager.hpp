#pragma once

#include "ECS/isystem.hpp"

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cassert>

class SystemManager{
public:
    template<typename T> void registerSystem(std::shared_ptr<T> sys);

    void addComponent(Entity entity,Signature newsign, ComponentType added);
    void removeComponent(Entity entity, ComponentType removed);
    void destroyEntity(Entity entity, Signature sign);
private:
    std::unordered_map<std::type_index, std::shared_ptr<ISystem>> m_systems{};
};


template<typename T> 
void SystemManager::registerSystem(std::shared_ptr<T> sys){
    static_assert(std::is_base_of_v<ISystem, T>, "T must inherit from ISystem.");
    std::type_index typeName = std::type_index(typeid(T));
    assert(m_systems.find(typeName) == m_systems.end() && "Registering system more than once.");

    m_systems.insert({typeName, sys});
}