#pragma once

#include "ECS/entity.hpp"

#include <memory>


class ComponentManager;
class SystemManager;

class Coordinator{
public:
	void init();


	// Entity methods
	Entity createEntity();
	void DestroyEntity(Entity entity);


	// Component methods
	template<typename T> void registerComponent();
	template<typename T> void addComponent(Entity entity, T component);
	template<typename T> void removeComponent(Entity entity);
	template<typename T> T& getComponent(Entity entity);
	template<typename T> ComponentType getComponentType();


	// System methods
	template<typename T> std::shared_ptr<T> registerSystem();
	template<typename T> void setSystemSignature(Signature signature);

private:
	std::unique_ptr<ComponentManager> mComponentManager;
	std::unique_ptr<EntityManager> mEntityManager;
	std::unique_ptr<SystemManager> mSystemManager;
};





template<typename T>
void Coordinator::registerComponent(){
    mComponentManager->registerComponent<T>();
}

template<typename T>
void Coordinator::addComponent(Entity entity, T component){
    mComponentManager->AddComponent<T>(entity, component);

    auto signature = mEntityManager->GetSignature(entity);
    signature.set(mComponentManager->getComponentType<T>(), true);
    mEntityManager->SetSignature(entity, signature);

    mSystemManager->EntitySignatureChanged(entity, signature);
}

template<typename T>
void Coordinator::removeComponent(Entity entity){
    mComponentManager->removeComponent<T>(entity);

    auto signature = mEntityManager->GetSignature(entity);
    signature.set(mComponentManager->getComponentType<T>(), false);
    mEntityManager->SetSignature(entity, signature);

    mSystemManager->EntitySignatureChanged(entity, signature);
}

template<typename T>
T& Coordinator::getComponent(Entity entity){
    return mComponentManager->getComponent<T>(entity);
}

template<typename T>
ComponentType Coordinator::getComponentType(){
    return mComponentManager->getComponentType<T>();
}


// System methods
template<typename T>
std::shared_ptr<T> Coordinator::registerSystem(){
    return mSystemManager->registerSystem<T>();
}

template<typename T>
void Coordinator::setSystemSignature(Signature signature){
    mSystemManager->SetSignature<T>(signature);
}
