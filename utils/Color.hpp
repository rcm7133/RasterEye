#pragma once

// Represents and RGB color scheme with values from 0-255
struct Color
{
    // Defgault to black
    uint8_t r{0}, g{0}, b{0};
    Color() = default;
    Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};