#pragma once

#include "System/keyboard.hpp"

#include <cstdint>
#include <string_view>
#include <unordered_map>

// 1. Correctly define the forward-declared enum outside the class
enum class Keyboard::Key : uint16_t {
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


inline const std::unordered_map<std::string_view, Keyboard::Key> Keyboard::StringToKeyMap = {
    // Printable
    {"Space", Keyboard::Key::Space}, {"Apostrophe", Keyboard::Key::Apostrophe}, 
    {"Comma", Keyboard::Key::Comma}, {"Minus", Keyboard::Key::Minus}, 
    {"Period", Keyboard::Key::Period}, {"Slash", Keyboard::Key::Slash},
    
    // Numbers
    {"D0", Keyboard::Key::D0}, {"D1", Keyboard::Key::D1}, {"D2", Keyboard::Key::D2}, {"D3", Keyboard::Key::D3}, 
    {"D4", Keyboard::Key::D4}, {"D5", Keyboard::Key::D5}, {"D6", Keyboard::Key::D6}, {"D7", Keyboard::Key::D7}, 
    {"D8", Keyboard::Key::D8}, {"D9", Keyboard::Key::D9},
    
    {"Semicolon", Keyboard::Key::Semicolon}, {"Equal", Keyboard::Key::Equal},
    
    // Letters
    {"A", Keyboard::Key::A}, {"B", Keyboard::Key::B}, {"C", Keyboard::Key::C}, {"D", Keyboard::Key::D}, 
    {"E", Keyboard::Key::E}, {"F", Keyboard::Key::F}, {"G", Keyboard::Key::G}, {"H", Keyboard::Key::H}, 
    {"I", Keyboard::Key::I}, {"J", Keyboard::Key::J}, {"K", Keyboard::Key::K}, {"L", Keyboard::Key::L}, 
    {"M", Keyboard::Key::M}, {"N", Keyboard::Key::N}, {"O", Keyboard::Key::O}, {"P", Keyboard::Key::P}, 
    {"Q", Keyboard::Key::Q}, {"R", Keyboard::Key::R}, {"S", Keyboard::Key::S}, {"T", Keyboard::Key::T}, 
    {"U", Keyboard::Key::U}, {"V", Keyboard::Key::V}, {"W", Keyboard::Key::W}, {"X", Keyboard::Key::X}, 
    {"Y", Keyboard::Key::Y}, {"Z", Keyboard::Key::Z},
    
    // Punctuation
    {"LeftBracket", Keyboard::Key::LeftBracket}, {"Backslash", Keyboard::Key::Backslash}, 
    {"RightBracket", Keyboard::Key::RightBracket}, {"Grave", Keyboard::Key::Grave},
    
    // Function keys
    {"Esc", Keyboard::Key::Esc}, {"Enter", Keyboard::Key::Enter}, {"Tab", Keyboard::Key::Tab}, 
    {"Backspace", Keyboard::Key::Backspace}, {"Insert", Keyboard::Key::Insert}, 
    {"Delete", Keyboard::Key::Delete}, {"Right", Keyboard::Key::Right}, {"Left", Keyboard::Key::Left}, 
    {"Down", Keyboard::Key::Down}, {"Up", Keyboard::Key::Up}, {"PageUp", Keyboard::Key::PageUp}, 
    {"PageDown", Keyboard::Key::PageDown}, {"Home", Keyboard::Key::Home}, {"End", Keyboard::Key::End}, 
    {"CapsLock", Keyboard::Key::CapsLock}, {"ScrollLock", Keyboard::Key::ScrollLock}, 
    {"NumLock", Keyboard::Key::NumLock}, {"PrintScreen", Keyboard::Key::PrintScreen}, 
    {"Pause", Keyboard::Key::Pause},
    
    {"F1", Keyboard::Key::F1}, {"F2", Keyboard::Key::F2}, {"F3", Keyboard::Key::F3}, {"F4", Keyboard::Key::F4}, 
    {"F5", Keyboard::Key::F5}, {"F6", Keyboard::Key::F6}, {"F7", Keyboard::Key::F7}, {"F8", Keyboard::Key::F8}, 
    {"F9", Keyboard::Key::F9}, {"F10", Keyboard::Key::F10}, {"F11", Keyboard::Key::F11}, {"F12", Keyboard::Key::F12},
    
    // Numpad
    {"Kp0", Keyboard::Key::Kp0}, {"Kp1", Keyboard::Key::Kp1}, {"Kp2", Keyboard::Key::Kp2}, {"Kp3", Keyboard::Key::Kp3}, 
    {"Kp4", Keyboard::Key::Kp4}, {"Kp5", Keyboard::Key::Kp5}, {"Kp6", Keyboard::Key::Kp6}, {"Kp7", Keyboard::Key::Kp7}, 
    {"Kp8", Keyboard::Key::Kp8}, {"Kp9", Keyboard::Key::Kp9}, {"KpDecimal", Keyboard::Key::KpDecimal}, 
    {"KpDivide", Keyboard::Key::KpDivide}, {"KpMultiply", Keyboard::Key::KpMultiply}, 
    {"KpSubtract", Keyboard::Key::KpSubtract}, {"KpAdd", Keyboard::Key::KpAdd}, 
    {"KpEnter", Keyboard::Key::KpEnter}, {"KpEqual", Keyboard::Key::KpEqual},
    
    // Modifiers
    {"LeftShift", Keyboard::Key::LeftShift}, {"LeftControl", Keyboard::Key::LeftControl}, 
    {"LeftAlt", Keyboard::Key::LeftAlt}, {"LeftSuper", Keyboard::Key::LeftSuper}, 
    {"RightShift", Keyboard::Key::RightShift}, {"RightControl", Keyboard::Key::RightControl}, 
    {"RightAlt", Keyboard::Key::RightAlt}, {"RightSuper", Keyboard::Key::RightSuper}
};