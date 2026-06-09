#pragma once

#include <cstdint>
#include <bitset>
#include <queue>
#include <array>


using Entity = uint32_t;
const Entity MAX_ENTITIES = 5000;

using ComponentType = uint8_t;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;



class EntityManager{
public:
	EntityManager();

	Entity createEntity();
	Entity createEntity(Signature signature);
	void destroyEntity(Entity entity);

	void setSignature(Entity entity, Signature signature);
	Signature getSignature(Entity entity);

private:
	std::queue<Entity> mAvailableEntities;
	std::array<Signature, MAX_ENTITIES> mSignatures;

	uint32_t mLivingEntityCount = 0;
};