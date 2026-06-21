#pragma once

#include "ECS/type.hpp"

#include <stack>
#include <array>

class EntityManager{
public:
    EntityManager();
    
    Entity createEntity();
    void destroyEntity(Entity entity);

    Signature getSignature(Entity entity);
    Signature setSignature(Entity entity, Signature sign);

    bool hasComponent(Entity entity, ComponentType type);
    Signature addComponentType(Entity entity, ComponentType type);
    Signature removeComponentType(Entity entity, ComponentType type);
private:
	std::stack<Entity> m_available_entities{};
	std::array<Signature, MAX_ENTITIES> m_signatures{};

	Entity m_living_entity_count = 0;
};