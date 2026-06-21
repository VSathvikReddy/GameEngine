#include "ECS/component_manager.hpp"

void ComponentManager::destroyEntity(Entity entity) {
    for (auto const& pair : m_component_arrays) {
        auto const& component = pair.second;
        component->destroyEntity(entity);
    }
}