#include <cassert>

#define VIEW_SCOPE View<FirstComp, OtherComps...>

// VIEW IMPLEMENTATION

template<typename FirstComp, typename... OtherComps>
VIEW_SCOPE::View(ECS& ecs_ref) : ecs(ecs_ref) {
    m_first_array = ecs.m_component_manager.template getComponentArray<CleanFirst>();
}

template<typename FirstComp, typename... OtherComps>
template<typename Func>
void VIEW_SCOPE::each(Func&& func) {
    size_t size = m_first_array->size();
    
    // FIXED: Now correctly pointing to the dense Entity ID array
    const auto& dense_entities = m_first_array->m_dense_entity_array;

    for (size_t i = 0; i < size; ++i) {
        Entity entity = dense_entities[i];
        
        if (ecs.hasSignature<FirstComp, OtherComps...>(entity)) {
            func(entity, 
                 static_cast<ReturnRef<FirstComp>>(m_first_array->getData(entity)), 
                 static_cast<ReturnRef<OtherComps>>(ecs.m_component_manager.template getComponentArray<std::remove_const_t<OtherComps>>()->getData(entity))...);
        }
    }
}

template<typename FirstComp, typename... OtherComps>
typename VIEW_SCOPE::Iterator VIEW_SCOPE::begin() {
    return Iterator(*this, 0);
}

template<typename FirstComp, typename... OtherComps>
typename VIEW_SCOPE::Iterator VIEW_SCOPE::end() {
    return Iterator(*this, m_first_array->size());
}


// ITERATOR IMPLEMENTATION

template<typename FirstComp, typename... OtherComps>
VIEW_SCOPE::Iterator::Iterator(View& view, size_t index) 
    : ecs(view.ecs), m_first_array(view.m_first_array), m_index(index) 
{
    skipInvalid();
}

template<typename FirstComp, typename... OtherComps>
std::tuple<Entity, 
           typename VIEW_SCOPE::template ReturnRef<FirstComp>, 
           typename VIEW_SCOPE::template ReturnRef<OtherComps>...> 
VIEW_SCOPE::Iterator::operator*() const {
    assert(m_index < m_first_array->size() && "Out of bound iterator being dereferenced for view");
    
    // FIXED: Grabbing the Entity ID instead of the component data
    Entity entity = m_first_array->m_dense_entity_array[m_index];
    
    return std::forward_as_tuple(
        entity,
        static_cast<ReturnRef<FirstComp>>(m_first_array->getData(entity)),
        static_cast<ReturnRef<OtherComps>>(ecs.m_component_manager.template getComponentArray<std::remove_const_t<OtherComps>>()->getData(entity))...
    );
}

template<typename FirstComp, typename... OtherComps>
typename VIEW_SCOPE::Iterator& VIEW_SCOPE::Iterator::operator++() {
    ++m_index;
    skipInvalid();
    
    return *this;
}

template<typename FirstComp, typename... OtherComps>
bool VIEW_SCOPE::Iterator::operator!=(const Iterator& other) const {
    return m_index != other.m_index;
}

template<typename FirstComp, typename... OtherComps>
void VIEW_SCOPE::Iterator::skipInvalid() {
    while (m_index < m_first_array->size()) {
        // FIXED: Checking the signature of the Entity ID
        if (ecs.hasSignature<FirstComp, OtherComps...>(m_first_array->m_dense_entity_array[m_index])) break; 
        ++m_index;
    }   
}

#undef VIEW_SCOPE