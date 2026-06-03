#pragma once

#include <vector>
#include <stack>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <optional>
#include <stdexcept>

template<typename T>
class FreeList {
private:
    struct Slot {
        std::optional<T> data; // Allocates byte space but DOES NOT call the constructor until insert
    };

    std::vector<Slot> m_storage;
    std::stack<uint32_t, std::vector<uint32_t>> m_freeIndices;
    size_t m_activeCount = 0;

public:
    // STANDARD LIBRARY TYPE ALIASES
    using value_type      = T;
    using size_type        = size_t;
    using difference_type = std::ptrdiff_t;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;

    // Forward declaration of our nested iterator implementation
    template<bool IsConst> class IteratorBase;

    using iterator       = IteratorBase<false>;
    using const_iterator = IteratorBase<true>;

    FreeList();
    ~FreeList() = default;


    uint32_t insert(const T& item);
    uint32_t insert(T&& item);
    void erase(uint32_t id);
    void clear();


    reference at(uint32_t id);
    const_reference at(uint32_t id) const;


    size_type size() const noexcept;
    size_type capacity() const noexcept;
    bool empty() const noexcept;


    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
};


template<typename T>
template<bool IsConst>
class FreeList<T>::IteratorBase {
private:
    using StoragePtr = std::conditional_t<IsConst, const std::vector<Slot>*, std::vector<Slot>*>;
    StoragePtr m_storage_ref;
    size_t m_index;

    void advanceToNextValid() noexcept;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = std::conditional_t<IsConst, const T*, T*>;
    using reference         = std::conditional_t<IsConst, const T&, T&>;

    IteratorBase(StoragePtr storage, size_t index) noexcept;

    reference operator*() const noexcept;
    pointer operator->() const noexcept;

    IteratorBase& operator++() noexcept;
    IteratorBase operator++(int) noexcept;

    bool operator==(const IteratorBase& other) const noexcept;
    bool operator!=(const IteratorBase& other) const noexcept;
};

template<typename T>
FreeList<T>::FreeList() {
    // Index 0 safely initialized as an empty optional wrapper. 
    // T's constructor is never triggered here.
    m_storage.push_back(Slot{ std::nullopt }); 
}

template<typename T>
uint32_t FreeList<T>::insert(const T& item) {
    uint32_t targetID = 0;
    if (!m_freeIndices.empty()) {
        targetID = m_freeIndices.top();
        m_freeIndices.pop();
        m_storage[targetID].data = item; // Copy assignment constructor
    } else {
        targetID = static_cast<uint32_t>(m_storage.size());
        m_storage.push_back(Slot{ item }); // Copy instantiation
    }
    m_activeCount++;
    return targetID;
}

template<typename T>
uint32_t FreeList<T>::insert(T&& item) {
    uint32_t targetID = 0;
    if (!m_freeIndices.empty()) {
        targetID = m_freeIndices.top();
        m_freeIndices.pop();
        m_storage[targetID].data = std::move(item); // Move assignment pipeline
    } else {
        targetID = static_cast<uint32_t>(m_storage.size());
        m_storage.push_back(Slot{ std::move(item) }); // Move instantiation
    }
    m_activeCount++;
    return targetID;
}

template<typename T>
void FreeList<T>::erase(uint32_t id) {
    if (id == 0 || id >= m_storage.size() || !m_storage[id].data.has_value()) return;
    
    // Instantly fires T's custom destructor and clears internal resource maps,
    // keeping the outer flat vector buffer available for future allocations.
    m_storage[id].data = std::nullopt; 
    
    m_freeIndices.push(id);
    m_activeCount--;
}

template<typename T>
void FreeList<T>::clear() {
    m_storage.clear();
    m_freeIndices = std::stack<uint32_t, std::vector<uint32_t>>();
    m_storage.push_back(Slot{ std::nullopt });
    m_activeCount = 0;
}

template<typename T>
typename FreeList<T>::reference FreeList<T>::at(uint32_t id) {
    if (id >= m_storage.size() || !m_storage[id].data.has_value()) {
        throw std::out_of_range("FreeList: Attempted to access an invalid or dead ID handle index.");
    }
    return m_storage[id].data.value();
}

template<typename T>
typename FreeList<T>::const_reference FreeList<T>::at(uint32_t id) const {
    if (id >= m_storage.size() || !m_storage[id].data.has_value()) {
        throw std::out_of_range("FreeList: Attempted to access an invalid or dead ID handle index.");
    }
    return m_storage[id].data.value();
}

template<typename T>
typename FreeList<T>::size_type FreeList<T>::size() const noexcept {
    return m_activeCount;
}

template<typename T>
typename FreeList<T>::size_type FreeList<T>::capacity() const noexcept {
    return m_storage.capacity();
}

template<typename T>
bool FreeList<T>::empty() const noexcept {
    return m_activeCount == 0;
}

// --- Iterator Setup Bridges ---

template<typename T>
typename FreeList<T>::iterator FreeList<T>::begin() noexcept { return iterator(&m_storage, 1); }

template<typename T>
typename FreeList<T>::iterator FreeList<T>::end() noexcept { return iterator(&m_storage, m_storage.size()); }

template<typename T>
typename FreeList<T>::const_iterator FreeList<T>::begin() const noexcept { return const_iterator(&m_storage, 1); }

template<typename T>
typename FreeList<T>::const_iterator FreeList<T>::end() const noexcept { return const_iterator(&m_storage, m_storage.size()); }

template<typename T>
typename FreeList<T>::const_iterator FreeList<T>::cbegin() const noexcept { return const_iterator(&m_storage, 1); }

template<typename T>
typename FreeList<T>::const_iterator FreeList<T>::cend() const noexcept { return const_iterator(&m_storage, m_storage.size()); }




template<typename T>
template<bool IsConst>
FreeList<T>::IteratorBase<IsConst>::IteratorBase(StoragePtr storage, size_t index) noexcept 
    : m_storage_ref(storage), m_index(index) {
    advanceToNextValid();
}

template<typename T>
template<bool IsConst>
void FreeList<T>::IteratorBase<IsConst>::advanceToNextValid() noexcept {
    // Continually loops until it catches a slot housing an initialized data property
    while (m_index < m_storage_ref->size() && !(*m_storage_ref)[m_index].data.has_value()) {
        m_index++;
    }
}

template<typename T>
template<bool IsConst>
typename FreeList<T>::template IteratorBase<IsConst>::reference 
FreeList<T>::IteratorBase<IsConst>::operator*() const noexcept { 
    return const_cast<reference>(*((*m_storage_ref)[m_index].data)); 
}

template<typename T>
template<bool IsConst>
typename FreeList<T>::template IteratorBase<IsConst>::pointer 
FreeList<T>::IteratorBase<IsConst>::operator->() const noexcept { 
    return const_cast<pointer>(&((*m_storage_ref)[m_index].data)); 
}

template<typename T>
template<bool IsConst>
typename FreeList<T>::template IteratorBase<IsConst>& 
FreeList<T>::IteratorBase<IsConst>::operator++() noexcept {
    m_index++;
    advanceToNextValid();
    return *this;
}

template<typename T>
template<bool IsConst>
typename FreeList<T>::template IteratorBase<IsConst> 
FreeList<T>::IteratorBase<IsConst>::operator++(int) noexcept {
    IteratorBase temp = *this;
    ++(*this);
    return temp;
}

template<typename T>
template<bool IsConst>
bool FreeList<T>::IteratorBase<IsConst>::operator==(const IteratorBase& other) const noexcept { 
    return m_index == other.m_index; 
}

template<typename T>
template<bool IsConst>
bool FreeList<T>::IteratorBase<IsConst>::operator!=(const IteratorBase& other) const noexcept { 
    return m_index != other.m_index; 
}