#pragma once

#include "ECS/isystem.hpp"
#include "ECS/ecs.hpp"

#include <vector>
#include <algorithm>

template<typename... Args>
class System : public ISystem {
public:
    System(ECS& ecs) : ISystem(ecs.getSignature<Args...>()) {}
};


template<template<typename...> class ContainerTemplate,     typename... Args>
class SimpleContainerSystem : public ISystem {
public:

    SimpleContainerSystem(ECS& ecs) : ISystem(ecs.getSignature<Args...>()) {}

    void addEntity(Entity entity) override {
        // 3. Compare the instantiated type
        if constexpr (std::is_same_v<ContainerTemplate<Entity>, std::vector<Entity>>) {
            m_entities.push_back(entity);
        } else {
            m_entities.insert(entity);
        }
    }

    void removeEntity(Entity entity) override {
        // 3. Compare the instantiated type
        if constexpr (std::is_same_v<ContainerTemplate<Entity>, std::vector<Entity>>) {
            // Swap and Pop for vectors!
            auto it = std::find(m_entities.begin(), m_entities.end(), entity);
            if (it != m_entities.end()) {
                *it = m_entities.back();
                m_entities.pop_back();
            }
        } else {
            m_entities.erase(entity);
        }
    }

protected:
    ContainerTemplate<Entity> m_entities;
};

// 4. Update the alias to also use the template template parameter
template<template<typename...> class ContainerTemplate, typename... Args>
using SSystem = SimpleContainerSystem<ContainerTemplate, Args...>;