#pragma once

#include "ECS/entity.hpp"

#include <cstdint>
#include <assert.h>
#include <unordered_map>
#include <array>
#include <memory>




class IComponentArray{
public:
	virtual ~IComponentArray() = default;
	virtual void entityDestroyed(Entity entity) = 0;
};


template<typename T>
class ComponentArray : public IComponentArray{
public:

	void insert(Entity entity, T component);
	void remove(Entity entity);
	T& getData(Entity entity);

	void destoryEntity(Entity entity) override;

private:
	std::array<T, MAX_ENTITIES> mComponentArray;

	std::unordered_map<Entity, size_t> mEntityToIndexMap;
	std::unordered_map<size_t, Entity> mIndexToEntityMap;

	size_t mSize;
};






class ComponentManager{
public:
	template<typename T> void registerComponent();
	template<typename T> ComponentType getComponentType();

	template<typename T> void addComponent(Entity entity, T component);
	template<typename T> void removeComponent(Entity entity);
	template<typename T> T& getComponent(Entity entity);

	void entityDestroyed(Entity entity);

private:
	// Map from type string pointer to a component type
	std::unordered_map<const char*, ComponentType> mComponentTypes{};

	// Map from type string pointer to a component array
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};

	// The component type to be assigned to the next registered component - starting at 0
	ComponentType mNextComponentType{};

	// Convenience function to get the statically casted pointer to the ComponentArray of type T.
	template<typename T> std::shared_ptr<ComponentArray<T>> getComponentArray();
};






























template<typename T>
void ComponentArray<T>::insert(Entity entity, T component){
    if(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()){
        throw std::runtime_error("Entity already exisits, but trying to insert");
    }

    size_t newIndex = mSize;
    mEntityToIndexMap.insert(entity,newIndex);
    mIndexToEntityMap.insert(newIndex, entity);
    mComponentArray.insert[newIndex] = component;
    ++mSize;
}

template<typename T>
void ComponentArray<T>::remove(Entity entity){
    if(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end()){
        throw std::runtime_error("Non existant Entity being tried to delete");
    }
    // Swap with last
    size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
    size_t indexOfLastElement = mSize - 1;
    mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

    // Update map
    Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
    mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
    mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

    // Remove from map
    mEntityToIndexMap.erase(entity);
    mIndexToEntityMap.erase(indexOfLastElement);

    --mSize;
}

template<typename T>
T& ComponentArray<T>::getData(Entity entity){
    if(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end()){
        throw std::runtime_error("Non existant Entity being tried to fetch data");
    }
    // Return a reference to the entity's component
    return mComponentArray[mEntityToIndexMap[entity]];
}

template<typename T> //For the virtual
void ComponentArray<T>::destoryEntity(Entity entity){
    if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end()){
        remove(entity);
    }
}
















template<typename T>
void ComponentManager::registerComponent(){
    const char* typeName = typeid(T).name();

    assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

    mComponentTypes.insert({typeName, mNextComponentType});
    mComponentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});

    ++mNextComponentType;
}

template<typename T>
ComponentType ComponentManager::getComponentType(){
    const char* typeName = typeid(T).name();

    assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

    return mComponentTypes[typeName];
}

template<typename T>
void ComponentManager::addComponent(Entity entity, T component){
    getComponentArray<T>()->insertData(entity, component);
}

template<typename T>
void ComponentManager::removeComponent(Entity entity){
    getComponentArray<T>()->removeData(entity);
}

template<typename T>
T& ComponentManager::getComponent(Entity entity){
    return getComponentArray<T>()->getData(entity);
}

void ComponentManager::entityDestroyed(Entity entity){
    for (auto const& pair : mComponentArrays){
        auto const& component = pair.second;
        component->entityDestroyed(entity);
    }
}

template<typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::getComponentArray(){
    const char* typeName = typeid(T).name();

    assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

    return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
}