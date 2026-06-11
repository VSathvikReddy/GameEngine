#pragma once

#include "ECS/component_array.hpp"

#include <cstddef>

template <typename Target, typename... AllTypes>
constexpr size_t get_type_index() {
    size_t index = 0;
    // Fold expression short-circuits upon finding a type match
    ((std::is_same_v<Target, AllTypes> ? true : (++index, false)) || ...);
    return index;
}




#define ECS_SCOPE ECS<AllComponents...>

template <typename... AllComponents>
class ECS {
public:
    using ComponentTypesTuple = std::tuple<AllComponents...>;
    static constexpr ComponentType MAX_COMPONENTS = sizeof...(AllComponents);
    using Signature = std::bitset<MAX_COMPONENTS>;

    ECS();

    Entity createEntity();
    Entity cloneEntity(Entity entity);
    void destroyEntity(Entity entity);

    template <typename... Args> static Signature getComponentSignature();
    template <typename T> static Signature addComponentSignature(Signature sign);
    Signature getSignature(Entity entity) const;

    template <typename... Args> bool hasComponent(Entity entity) const;
    template <typename... Args> void addComponents(Entity entity, Args... components);
    template <typename... Args> void modifyComponents(Entity entity, Args... components);
    template <typename... Args> void removeComponent(Entity entity);
    template <typename T> T& getComponent(Entity entity);

private:
    std::array<Signature, MAX_ENTITIES> m_signature_array{};
    std::vector<Entity> m_free_entities;
    
    std::tuple<ComponentArray<AllComponents>...> mComponentArrays;
    template <typename Func> void forEachArray(Func&& func);
};


template <typename... AllComponents>
ECS_SCOPE::ECS(){
    m_free_entities.reserve(MAX_ENTITIES);
    for (Entity i = MAX_ENTITIES; i > 0; i--){
        m_free_entities.push_back(i - 1);
    }
}

template <typename... AllComponents>
Entity ECS_SCOPE::createEntity(){
    assert(!m_free_entities.empty() && "EntityManager out of available entity IDs.");
    Entity id = m_free_entities.back();
    m_free_entities.pop_back();
    m_signature_array[id].reset();  // Safety
    return id;
}

template <typename... AllComponents>
Entity ECS_SCOPE::cloneEntity(Entity entity) {
    assert(entity < MAX_ENTITIES && "Entity out of bounds index.");
    Entity newId = createEntity();
    m_signature_array[newId] = m_signature_array[entity];
    

    forEachArray(
        [&](auto& array, std::size_t index) {
            if (m_signature_array[entity].test(index)) {
                array.insert(newId, array.getData(entity));
            }
        }
    );
    
    return newId;
}

template <typename... AllComponents>
void ECS_SCOPE::destroyEntity(Entity entity) {
    assert(entity < MAX_ENTITIES && "Entity out of bounds index.");

    forEachArray(
        [&](auto& array, std::size_t index) {
            if (m_signature_array[entity].test(index)) {
                array.remove(entity);
            }
        }
    );

    m_signature_array[entity].reset();
    m_free_entities.push_back(entity);
}










template <typename... AllComponents>
template <typename... Args> 
typename ECS_SCOPE::Signature ECS_SCOPE::getComponentSignature() {
    Signature sig;
    (sig.set(get_type_index<Args, AllComponents...>(), true), ...);
    return sig;
}

template <typename... AllComponents>
template <typename T> 
typename ECS_SCOPE::Signature ECS_SCOPE::addComponentSignature(Signature sign) {
    sign.set(get_type_index<T, AllComponents...>(), true);
    return sign;
}

template <typename... AllComponents>
typename ECS_SCOPE::Signature ECS_SCOPE::getSignature(Entity entity) const {
    assert(entity < MAX_ENTITIES && "Entity out of bounds index.");
    return m_signature_array[entity];
}












template <typename... AllComponents>
template <typename... Args>
bool ECS_SCOPE::hasComponent(Entity entity) const {
    assert(entity < MAX_ENTITIES && "Entity out of bounds index.");
    Signature mask = getComponentSignature<Args...>();
    
    return (m_signature_array[entity] & mask) == mask;
}


template <typename... AllComponents>
template <typename... Args>
void ECS_SCOPE::addComponents(Entity entity, Args... components) {
    assert(entity < MAX_ENTITIES && "Entity out of bounds index.");
    Signature newMask = getComponentSignature<Args...>();
    assert( (m_signature_array[entity] & newMask).none() && "Entity already has one or more of these components!");

    ([&]() {
        std::get<ComponentArray<Args>>(mComponentArrays).insert(entity, components);
    }(), ...); // Lamda unpacking safe

    m_signature_array[entity] |= newMask;

    // 4. Notify the System Manager EXACTLY ONCE
    // mSystemManager->EntitySignatureChanged(entity, m_signature_array[entity]);
}


template <typename... AllComponents>
template <typename... Args>
void ECS_SCOPE::modifyComponents(Entity entity, Args... components) {
#ifndef NDEBUG
    bool hasAllRequiredComponents = hasComponent<Args...>(entity);
    assert(hasAllRequiredComponents && "Entity does not have all these components to remove!");
#endif

    ([&]() {
        std::get<ComponentArray<Args>>(mComponentArrays).getData(entity) = components;   
    }(), ...);

}

template <typename... AllComponents>
template <typename... Args> 
void ECS_SCOPE::removeComponent(Entity entity) {
#ifndef NDEBUG
    bool hasAllRequiredComponents = hasComponent<Args...>(entity);
    assert(hasAllRequiredComponents && "Entity does not have all these components to remove!");
#endif
    
    Signature removalMask = getComponentSignature<Args...>();
    m_signature_array[entity] &= ~removalMask;
    
    ([&]() {
        std::get<ComponentArray<Args>>(mComponentArrays).remove(entity);
    }(), ...);
}

template <typename... AllComponents>
template <typename T> 
T& ECS_SCOPE::getComponent(Entity entity) {
    assert(hasComponent<T>(entity) && "Retrieving non-existent component!");
    return std::get<ComponentArray<T>>(mComponentArrays).getData(entity);
}



template <typename... AllComponents>
template <typename Func>
void ECS_SCOPE::forEachArray(Func&& func) {
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        // Unrolls into executing the lambda function for every single tuple index
        ([&]() {
            func(std::get<Is>(mComponentArrays), Is);
        }(), ...);
    }(std::make_index_sequence<MAX_COMPONENTS>{});
}
#undef ECS_SCOPE