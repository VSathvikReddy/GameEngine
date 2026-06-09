#include "ECS/entity.hpp"

#include <iostream>

EntityManager::EntityManager(){
    for (Entity entity = 0; entity < MAX_ENTITIES; entity++){
        mAvailableEntities.push(entity);
    }
}

Entity EntityManager::createEntity(){
	if (mLivingEntityCount >= MAX_ENTITIES) {
        throw std::runtime_error("EntityManager out of available entity IDs.");
    }
	

    Entity id = mAvailableEntities.front();
    mAvailableEntities.pop();
    ++mLivingEntityCount;


    return id;
}

Entity EntityManager::createEntity(Signature signature){
	if (mLivingEntityCount >= MAX_ENTITIES) {
        throw std::runtime_error("EntityManager out of available entity IDs.");
    }
	

    Entity id = mAvailableEntities.front();
    mAvailableEntities.pop();
    ++mLivingEntityCount;


	mSignatures[id] = signature;


    return id;
}

void EntityManager::destroyEntity(Entity entity){
	if (mLivingEntityCount >= MAX_ENTITIES) {
        throw std::runtime_error("EntityManager out of available entity IDs.");
    }


    mSignatures[entity].reset();


    mAvailableEntities.push(entity);
    --mLivingEntityCount;
}

void EntityManager::setSignature(Entity entity, Signature signature){
	if (mLivingEntityCount >= MAX_ENTITIES) {
        throw std::runtime_error("EntityManager out of available entity IDs.");
    }


    mSignatures[entity] = signature;
}

Signature EntityManager::getSignature(Entity entity){
	if (mLivingEntityCount >= MAX_ENTITIES) {
        throw std::runtime_error("EntityManager out of available entity IDs.");
    }
	

    return mSignatures[entity];
}