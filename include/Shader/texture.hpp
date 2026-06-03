#pragma once


#include <cstdint>
#include <string_view>


class TextureManager;

class Texture {
public:
    Texture() = delete;
    ~Texture() = default;

    uint32_t getWidth() const noexcept;
    uint32_t getHeight() const noexcept;

    void use(unsigned int slot = 0) const noexcept;

    [[nodiscard]] static Texture load(std::string_view path);

private:
    friend class TextureManager;
    constexpr explicit Texture(uint32_t id) noexcept : m_id(id) {}

    uint32_t m_id;
};