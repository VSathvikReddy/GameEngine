#pragma once

#include <cstdint>
#include <string_view>
#include <unordered_map>

// These values map exactly with GLFW keys
enum class Key : std::uint16_t {
    // Printable keys
    Space         = 32,
    Apostrophe    = 39, 
    Comma         = 44,
    Minus         = 45,
    Period        = 46,
    Slash         = 47, 
    
    // Numbers
    D0            = 48,
    D1            = 49,
    D2            = 50,
    D3            = 51,
    D4            = 52,
    D5            = 53,
    D6            = 54,
    D7            = 55,
    D8            = 56,
    D9            = 57,
    
    Semicolon     = 59,
    Equal         = 61,
    
    // Letters
    A             = 65,
    B             = 66,
    C             = 67,
    D             = 68,
    E             = 69,
    F             = 70,
    G             = 71,
    H             = 72,
    I             = 73,
    J             = 74,
    K             = 75,
    L             = 76,
    M             = 77,
    N             = 78,
    O             = 79,
    P             = 80,
    Q             = 81,
    R             = 82,
    S             = 83,
    T             = 84,
    U             = 85,
    V             = 86,
    W             = 87,
    X             = 88,
    Y             = 89,
    Z             = 90,
    
    // Punctuation
    LeftBracket   = 91, 
    Backslash     = 92, 
    RightBracket  = 93, 
    Grave         = 96, 
    
    // Function keys
    Esc           = 256,
    Enter         = 257,
    Tab           = 258,
    Backspace     = 259,
    Insert        = 260,
    Delete        = 261,
    Right         = 262,
    Left          = 263,
    Down          = 264,
    Up            = 265,
    PageUp        = 266,
    PageDown      = 267,
    Home          = 268,
    End           = 269,
    CapsLock      = 280,
    ScrollLock    = 281,
    NumLock       = 282,
    PrintScreen   = 283,
    Pause         = 284,
    
    F1            = 290,
    F2            = 291,
    F3            = 292,
    F4            = 293,
    F5            = 294,
    F6            = 295,
    F7            = 296,
    F8            = 297,
    F9            = 298,
    F10           = 299,
    F11           = 300,
    F12           = 301,
    
    // Numpad
    Kp0           = 320,
    Kp1           = 321,
    Kp2           = 322,
    Kp3           = 323,
    Kp4           = 324,
    Kp5           = 325,
    Kp6           = 326,
    Kp7           = 327,
    Kp8           = 328,
    Kp9           = 329,
    KpDecimal     = 330,
    KpDivide      = 331,
    KpMultiply    = 332,
    KpSubtract    = 333,
    KpAdd         = 334,
    KpEnter       = 335,
    KpEqual       = 336,
    
    // Modifiers
    LeftShift     = 340,
    LeftControl   = 341,
    LeftAlt       = 342,
    LeftSuper     = 343,
    RightShift    = 344,
    RightControl  = 345,
    RightAlt      = 346,
    RightSuper    = 347,

    //Is not bound so should return false
    ERROR = 0,
};

// 1. The inline map
inline const std::unordered_map<std::string_view, Key> StringToKeyMap = {
    // Printable
    {"Space", Key::Space}, {"Apostrophe", Key::Apostrophe}, 
    {"Comma", Key::Comma}, {"Minus", Key::Minus}, 
    {"Period", Key::Period}, {"Slash", Key::Slash},
    
    // Numbers
    {"D0", Key::D0}, {"D1", Key::D1}, {"D2", Key::D2}, {"D3", Key::D3}, 
    {"D4", Key::D4}, {"D5", Key::D5}, {"D6", Key::D6}, {"D7", Key::D7}, 
    {"D8", Key::D8}, {"D9", Key::D9},
    
    {"Semicolon", Key::Semicolon}, {"Equal", Key::Equal},
    
    // Letters
    {"A", Key::A}, {"B", Key::B}, {"C", Key::C}, {"D", Key::D}, 
    {"E", Key::E}, {"F", Key::F}, {"G", Key::G}, {"H", Key::H}, 
    {"I", Key::I}, {"J", Key::J}, {"K", Key::K}, {"L", Key::L}, 
    {"M", Key::M}, {"N", Key::N}, {"O", Key::O}, {"P", Key::P}, 
    {"Q", Key::Q}, {"R", Key::R}, {"S", Key::S}, {"T", Key::T}, 
    {"U", Key::U}, {"V", Key::V}, {"W", Key::W}, {"X", Key::X}, 
    {"Y", Key::Y}, {"Z", Key::Z},
    
    // Punctuation
    {"LeftBracket", Key::LeftBracket}, {"Backslash", Key::Backslash}, 
    {"RightBracket", Key::RightBracket}, {"Grave", Key::Grave},
    
    // Function keys
    {"Esc", Key::Esc}, {"Enter", Key::Enter}, {"Tab", Key::Tab}, 
    {"Backspace", Key::Backspace}, {"Insert", Key::Insert}, 
    {"Delete", Key::Delete}, {"Right", Key::Right}, {"Left", Key::Left}, 
    {"Down", Key::Down}, {"Up", Key::Up}, {"PageUp", Key::PageUp}, 
    {"PageDown", Key::PageDown}, {"Home", Key::Home}, {"End", Key::End}, 
    {"CapsLock", Key::CapsLock}, {"ScrollLock", Key::ScrollLock}, 
    {"NumLock", Key::NumLock}, {"PrintScreen", Key::PrintScreen}, 
    {"Pause", Key::Pause},
    
    {"F1", Key::F1}, {"F2", Key::F2}, {"F3", Key::F3}, {"F4", Key::F4}, 
    {"F5", Key::F5}, {"F6", Key::F6}, {"F7", Key::F7}, {"F8", Key::F8}, 
    {"F9", Key::F9}, {"F10", Key::F10}, {"F11", Key::F11}, {"F12", Key::F12},
    
    // Numpad
    {"Kp0", Key::Kp0}, {"Kp1", Key::Kp1}, {"Kp2", Key::Kp2}, {"Kp3", Key::Kp3}, 
    {"Kp4", Key::Kp4}, {"Kp5", Key::Kp5}, {"Kp6", Key::Kp6}, {"Kp7", Key::Kp7}, 
    {"Kp8", Key::Kp8}, {"Kp9", Key::Kp9}, {"KpDecimal", Key::KpDecimal}, 
    {"KpDivide", Key::KpDivide}, {"KpMultiply", Key::KpMultiply}, 
    {"KpSubtract", Key::KpSubtract}, {"KpAdd", Key::KpAdd}, 
    {"KpEnter", Key::KpEnter}, {"KpEqual", Key::KpEqual},
    
    // Modifiers
    {"LeftShift", Key::LeftShift}, {"LeftControl", Key::LeftControl}, 
    {"LeftAlt", Key::LeftAlt}, {"LeftSuper", Key::LeftSuper}, 
    {"RightShift", Key::RightShift}, {"RightControl", Key::RightControl}, 
    {"RightAlt", Key::RightAlt}, {"RightSuper", Key::RightSuper}
};