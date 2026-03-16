#pragma once

#include "utils/Math.hpp"
#include <cstdint>
#include "utils/float2.hpp"
#include "utils/Color.hpp"

// Represents a triangle with 3 points,
class Triangle
{
public:
    // Delete default constructor
    Triangle() = delete;
    // Constructor with all points, normals, ect...
    Triangle(float2 p1, float2 p2, float2 p3,
             float z1, float z2, float z3,
             float3 n1, float3 n2, float3 n3,
             float3 wp1, float3 wp2, float3 wp3)
           : p1(p1), p2(p2), p3(p3),
             z1(z1), z2(z2), z3(z3),
             n1(n1), n2(n2), n3(n3),
             wp1(wp1), wp2(wp2), wp3(wp3) {}
    // Set he color f the triangle
    void SetColor(const Color& c) { color = c; }
    // Get color of triangle
    const Color& GetColor() const { return color; }
    // Check if a point is inside of the triangle.
    bool IsPointInside(const float2& testPoint) const
    {
        // A point is only inside of a triangle if the point is to the right of the plane formed from the line X->Y for
        // all sides of the triangle
        bool sideAB = PointOnRightSideOfLine(p1, p2, testPoint);
        bool sideBC = PointOnRightSideOfLine(p2, p3, testPoint);
        bool sideCA = PointOnRightSideOfLine(p3, p1, testPoint);
        // CW and CCW compatible
        return (sideAB && sideBC && sideCA)       // clockwise
            || (!sideAB && !sideBC && !sideCA);   // counter-clockwise
    }
    // Interpolate the depth barycentricly given a point
    float InterpolateZ(const float2& point) const
    {
        float3 bary = Barycentric(point, p1, p2, p3);
        return bary.x * z1 + bary.y * z2 + bary.z * z3;
    }
    // Interpoate the normal barycentricly given a point
    float3 InterpolateNormal(const float2& point) const
    {
        float3 bary = Barycentric(point, p1, p2, p3);
        return {
            bary.x * n1.x + bary.y * n2.x + bary.z * n3.x,
            bary.x * n1.y + bary.y * n2.y + bary.z * n3.y,
            bary.x * n1.z + bary.y * n2.z + bary.z * n3.z
        };
    }
    // Intepolate the world position barycentricly given a point
    float3 InterpolateWorldPos(const float2& point) const
    {
        float3 bary = Barycentric(point, p1, p2, p3);
        return {
            bary.x * wp1.x + bary.y * wp2.x + bary.z * wp3.x,
            bary.x * wp1.y + bary.y * wp2.y + bary.z * wp3.y,
            bary.x * wp1.z + bary.y * wp2.z + bary.z * wp3.z
        };
    }
    // Get the bounding box for a triangle
    void GetBounds(int& minX, int& minY, int& maxX, int& maxY, int width, int height) const
    {
        minX = (int)std::max(0.0f,        std::min({p1.x, p2.x, p3.x}));
        minY = (int)std::max(0.0f,        std::min({p1.y, p2.y, p3.y}));
        maxX = (int)std::min((float)width,  std::max({p1.x, p2.x, p3.x}));
        maxY = (int)std::min((float)height, std::max({p1.y, p2.y, p3.y}));
    }

private:
    float2 p1, p2, p3;                 /// 2D screen space positons of triangle corners
    float z1, z2, z3;                  /// World space depth of each point
    float3 n1, n2, n3;                 /// Normals of each vertex
    float3 wp1, wp2, wp3;              /// World positon of each vertex
    Color color;                       /// Color of triangle
};

