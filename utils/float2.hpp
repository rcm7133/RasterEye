#pragma once

// Represents a 2d point (x, y)
struct float2
{
    float x{}, y{};

    float2() = default;
    float2(float x, float y) : x(x), y(y) {}
};

// Represents a 3d point (x, y, z)
struct float3
{
    float x{}, y{}, z{};

    float3() = default;
    float3(float x, float y, float z) : x(x), y(y), z(z) {}
    float3 operator+(const float3& o) const { return {x+o.x, y+o.y, z+o.z}; }
};