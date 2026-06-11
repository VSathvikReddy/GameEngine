class ISystemBase{
public:
    virtual bool match() = 0;
private:

};


template<typename... Components>
class ISystem{
public:
    template <typename... AllComponents>
    ISystem(ECS<AllComponents...> ecs);
};