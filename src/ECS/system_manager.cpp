#include "ECS/system_manager.hpp"

void SystemManager::addComponent(Entity entity,Signature newsign, ComponentType added){
    for(auto& [idx,sys]:m_systems){
        if (sys->target.test(added) && ((newsign & sys->target) == sys->target)) {
            sys->addEntity(entity);
        }
    }
}
void SystemManager::removeComponent(Entity entity, ComponentType removed){
    for (auto const& [idx, sys] : m_systems) {
        if (sys->target.test(removed)) {
            sys->removeEntity(entity);
        }
    }
}
void SystemManager::destroyEntity(Entity entity, Signature sign){
    for (auto const& [idx, sys] : m_systems) {
        if ((sign & sys->target) == sys->target) {    
            sys->removeEntity(entity);
        }
    }
}