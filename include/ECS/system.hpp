#pragma once

#include "ECS/entity.hpp"

#include <memory>
#include <unordered_map>
#include <set>


class System{
public:
	std::set<Entity> mEntities;
};

class SystemManager{
public:
	template<typename T> std::shared_ptr<T> registerSystem();

	template<typename T> void setSignature(Signature signature);

	void entityDestroyed(Entity entity);
	void entitySignatureChanged(Entity entity, Signature entitySignature);

private:
	std::unordered_map<const char*, Signature> mSignatures{};
	std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};
};







template<typename T>
std::shared_ptr<T> SystemManager::registerSystem(){
    const char* typeName = typeid(T).name();

    assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

    // Create a pointer to the system and return it so it can be used externally
    auto system = std::make_shared<T>();
    mSystems.insert({typeName, system});
    return system;
}

template<typename T>
void SystemManager::setSignature(Signature signature){
    const char* typeName = typeid(T).name();

    assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");

    // Set the signature for this system
    mSignatures.insert({typeName, signature});
}