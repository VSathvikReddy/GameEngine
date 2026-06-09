#include "ECS/coordinator.hpp"

#include "ECS/component.hpp"
#include "ECS/system.hpp"


void Coordinator::init(){
    mComponentManager = std::make_unique<ComponentManager>();
    mEntityManager = std::make_unique<EntityManager>();
    mSystemManager = std::make_unique<SystemManager>();
}


Entity Coordinator::createEntity(){
    return mEntityManager->createEntity();
}

void Coordinator::DestroyEntity(Entity entity){
    mEntityManager->destroyEntity(entity);
    mComponentManager->entityDestroyed(entity);
    mSystemManager->entityDestroyed(entity);
}