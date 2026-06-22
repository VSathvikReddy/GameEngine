#pragma once

#include "type.hpp"

class SystemManager;
class ISystem{
public:
    virtual ~ISystem() = default;
    virtual void addEntity(Entity entity) = 0;
    virtual void removeEntity(Entity entity) = 0;

protected:
    ISystem(Signature sig):target(sig) {}
private:
    friend class SystemManager;
    Signature target;
};