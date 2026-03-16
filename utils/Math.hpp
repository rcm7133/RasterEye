#pragma once
#include "float2.hpp"
#include <cmath>

// Subtract 2d points
inline float2 operator-(const float2& a, const float2& b)
{
    return { a.x - b.x, a.y - b.y };
}
// Dot product of 2d points
static float Dot2(const float2 &p1, const float2 &p2)
{
    return p1.x * p2.x + p1.y * p2.y;
}
// Dot product of 3d points
static float Dot3(const float3 &p1, const float3 &p2)
{
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}
// Rotates 2d vector by 90 degrees CCW
static float2 Perpindicular(const float2& point)
{
    return { -point.y, point.x };
}
// Checks if a given test point is on the right side of a line a->b
static bool PointOnRightSideOfLine(const float2& a, const float2& b, const float2& testPoint)
{
    float2 ap = testPoint - a;
    float2 abPerp = Perpindicular(b - a);
    return Dot2(ap, abPerp) >= 0;
}
// Projection based on distance (perspective projection)
static float2 Project(const float3& p)
{
    return float2(p.x / p.z, p.y / p.z);
}
// Normalize 3d vector
static float3 Normalized(const float3& v)
{
    float len = sqrtf(Dot3(v, v));
    if (len > 0.0f)
        return { v.x/len, v.y/len, v.z/len };
    return v;
}
// Convert point 2d point to screen coordinates
static float2 ToScreenCoords(const float2& p, int width, int height)
{
    return float2(
        (p.x + 1.0f) * 0.5f * (width - 1),
        (p.y + 1.0f) * 0.5f * (height - 1)  // leave flip for SetPixel
    );
}
// Rotation matrix X component
static float3 RotateX(const float3& v, float radians)
{
    float c = cosf(radians), s = sinf(radians);
    return { v.x, v.y * c - v.z * s, v.y * s + v.z * c };
}
// Rotation matrix Y component
static float3 RotateY(const float3& v, float radians)
{
    float c = cosf(radians), s = sinf(radians);
    return { v.x * c + v.z * s, v.y, -v.x * s + v.z * c };
}
// Rotation matrix Z component
static float3 RotateZ(const float3& v, float radians)
{
    float c = cosf(radians), s = sinf(radians);
    return { v.x * c - v.y * s, v.x * s + v.y * c, v.z };
}
// How close a given point p is from each point a, b, c. Return values are from 0-1.0 telling how close p is from a given
// point in a triangle
static float3 Barycentric(const float2& p, const float2& a, const float2& b, const float2& c)
{
    float denom = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);
    float w1 = ((b.y - c.y) * (p.x - c.x) + (c.x - b.x) * (p.y - c.y)) / denom;
    float w2 = ((c.y - a.y) * (p.x - c.x) + (a.x - c.x) * (p.y - c.y)) / denom;
    float w3 = 1.0f - w1 - w2;
    return { w1, w2, w3 };
}