#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>

struct TransparentStringHash {
    using is_transparent = void;

    [[nodiscard]] std::size_t operator()(std::string_view txt) const noexcept {
        return std::hash<std::string_view>{}(txt);
    }
};


template <typename ValueType>
using string_hash_map = std::unordered_map<
    std::string, 
    ValueType, 
    TransparentStringHash, 
    std::equal_to<>
>;
