#include "ECS/ecs.hpp"

Entity ECS::createEntity(){
    return m_entity_manager.createEntity();
}

void ECS::destroyEntity(Entity entity){
    m_system_manager.destroyEntity(entity, m_entity_manager.getSignature(entity));
    m_entity_manager.destroyEntity(entity);
    m_component_manager.destroyEntity(entity);
}
Signature ECS::getSignature(Entity entity){
    return m_entity_manager.getSignature(entity);
}