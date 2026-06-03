#pragma once

class KeyBind{
public:
    KeyBind(const char* file);

    Keyboard::Key find(uint32_t id);


    constexpr static uint32_t ID(std::string_view str);
private:
    std::unordered_map<uint32_t,Keyboard::Key> bindings;
};

constexpr uint32_t KeyBind::ID(std::string_view str){
    uint32_t hash = 2166136261u;
    for (char c : str) {
        hash ^= static_cast<uint32_t>(c);
        hash *= 16777619u;
    }
    return hash;
}