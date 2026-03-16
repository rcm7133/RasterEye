#pragma once
#include "../utils/float2.hpp"
#include "../utils/Color.hpp"

// Represents a light source
struct Light
{
    float3 position;
    Color color;
};