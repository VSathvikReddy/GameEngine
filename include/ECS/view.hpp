#pragma once

#include "ECS/type.hpp"

#include <type_traits>
#include <memory>
#include <tuple>

class ECS;
template<typename T> class ComponentArray;

template<typename FirstComp, typename... OtherComps>
class View {
    template<typename T> using Clean = std::remove_const_t<T>;
    template<typename T> using ReturnRef = std::conditional_t<std::is_const_v<T>, const std::remove_const_t<T>&, std::remove_const_t<T>&>;
public:
    View(const View&) = delete;  // Do not save or store it
    View& operator=(const View&) = delete;
    View(View&&) = delete;
    View& operator=(View&&) = delete;
    
    template<typename Func> void each(Func&& func); // Do not add or remove components while iterating

    class Iterator{
    public:
        Iterator(View& view, size_t index);

        // You can use ReturnRef completely raw!
        std::tuple<Entity, ReturnRef<FirstComp>, ReturnRef<OtherComps>...> operator*() const;

        Iterator& operator++();
        bool operator!=(const Iterator& other) const;

    private:
        ECS& ecs;
        std::shared_ptr<ComponentArray<Clean<FirstComp>>> m_first_array;
        size_t m_index;

        void skipInvalid();
    };

    Iterator begin();
    Iterator end(); // Do not add or remove components while iterating

private:
    ECS& ecs;
    std::shared_ptr<ComponentArray<Clean<FirstComp>>> m_first_array;

    View(ECS& ecs);
    friend class ECS;
};